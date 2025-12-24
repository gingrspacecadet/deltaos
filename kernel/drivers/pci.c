#include <arch/types.h>
#include <arch/io.h>

typedef struct {
    struct {
        bool is_io;
        uint8 size;
        uint32 addr;
    } bar[6];
    uint16 vendor, device, status, command;
    uint8 headertype, class, subclass, prog_if, int_line;
} pci_device;

static struct pci_out {
    pci_device pci;
    uint8 bus, dev, func;
};

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

static uint32 pci_config_read_dword(uint8 bus, uint8 slot, uint8 func, uint8 offset) {
    uint32 address;
    uint32 lbus  = (uint32)bus;
    uint32 lslot = (uint32)slot;
    uint32 lfunc = (uint32)func;
    uint32 tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32)0x80000000));
  
    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    tmp = inl(CONFIG_DATA);
    return tmp;
}

static void pci_config_write_dword(uint8 bus, uint8 slot, uint8 func, uint8 offset, uint32 value) {
    uint32 address;
    uint32 lbus  = (uint32)bus;
    uint32 lslot = (uint32)slot;
    uint32 lfunc = (uint32)func;
    uint32 tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32)0x80000000));
  
    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    outl(CONFIG_DATA, value);
}

#define get_byte(dword, offset) ((dword >> ((offset & 3) * 8)) & 0xFF) 
#define get_word(dword, offset) ((dword >> ((offset & 2) * 8)) & 0xFFFF) 

static int pci_probe_device(uint8 bus, uint8 dev, uint8 func, pci_device* out) {
    pci_device pci = {0};

    pci.vendor = get_word(pci_config_read_dword(bus, dev, func, 0x00), 0);
    pci.device = get_word(pci_config_read_dword(bus, dev, func, 0x00), 2);
    if (pci.vendor == 0xFFFF) return -1;

    
    pci.status      = get_word(pci_config_read_dword(bus, dev, func, 0x04), 2);
    pci.command     = get_word(pci_config_read_dword(bus, dev, func, 0x04), 0);
    
    pci.class       = get_byte(pci_config_read_dword(bus, dev, func, 0x08), 3);
    pci.subclass    = get_byte(pci_config_read_dword(bus, dev, func, 0x08), 2);
    pci.prog_if     = get_byte(pci_config_read_dword(bus, dev, func, 0x08), 1);
    
    pci.headertype = get_byte(pci_config_read_dword(bus, dev, func, 0x0C), 2);

    if (pci.headertype == 0x0) {

        pci.bar[0].addr        = pci_config_read_dword(bus, dev, func, 0x10);
        pci.bar[1].addr        = pci_config_read_dword(bus, dev, func, 0x14);
        pci.bar[2].addr        = pci_config_read_dword(bus, dev, func, 0x18);
        pci.bar[3].addr        = pci_config_read_dword(bus, dev, func, 0x1C);
        pci.bar[4].addr        = pci_config_read_dword(bus, dev, func, 0x20);
        pci.bar[5].addr        = pci_config_read_dword(bus, dev, func, 0x24);
        
        pci.int_line    = get_byte(pci_config_read_dword(bus, dev, func, 0x3C), 1);
    }

    *out = pci;

    return 0;
}

static int pci_probe_bars(uint8 bus, uint8 dev, uint8 func, pci_device* out) {
    for (int i = 0; i < 6; i++) {
        uint8 offset = 0x10 + (i * 4);
        uint32 orig = pci_config_read_dword(bus, dev, func, offset);

        // Skip if there is nothing to read lmao
        if (orig == 0) {
            out->bar[i].size = 0;
            out->bar[i].is_io = 0;
            continue;
        }

        // apparently this determines size idk ty wiki
        pci_config_write_dword(bus, dev, func, offset, 0xFFFFFFFF);
        uint32 probe = pci_config_read_dword(bus, dev, func, offset);

        pci_config_write_dword(bus, dev, func, offset, orig);

        // Skip if size is 0
        if (probe == 0) {
            out->bar[i].size = 0;
            out->bar[i].is_io = 0;
            continue;
        }

        if (probe & 0x1) {  // I/O space BAR
            out->bar[i].is_io = true;
            out->bar[i].size = ~(probe & 0xFFFFFFFC) + 1;   // sorcery
            out->bar[i].addr = orig & 0xFFFFFFFC;
        } else {
            // Memory space BAR
            uint32 type = (probe >> 1) & 0x3;
            if (type == 0x2) {
                // 64-bit BAR: combine with next BAR
                if (i + 1 < 6) {
                    uint8 next_offset = 0x10 + ((i + 1) * 4);
                    uint32 orig_hi = pci_config_read_dword(bus, dev, func, next_offset);

                    // same magic as earlier
                    pci_config_write_dword(bus, dev, func, next_offset, 0xFFFFFFFF);
                    uint32 probe_hi = pci_config_read_dword(bus, dev, func, next_offset);

                    // restore upper original
                    pci_config_write_dword(bus, dev, func, next_offset, orig_hi);

                    uint64 full_probe = ((uint64)probe_hi << 32) | (uint64)probe;

                    // mask lower 4 bits of low dword per spec
                    uint64 mask = ~(full_probe & (~(uint64)0xFULL));
                    uint64 size = mask + 1;

                    uint64 full_orig = ((uint64)orig_hi << 32) | (uint64)orig;
                    out->bar[i].is_io = false;
                    out->bar[i].size = (uint32)size;
                    out->bar[i].addr = (uint32)(full_orig & (~(uint64)0xFULL));

                    // mark next BAR as consumed
                    out->bar[i+1].size = 0;
                    out->bar[i+1].is_io = 0;
                    out->bar[i+1].addr = 0;

                    i++; // skip next BAR (upper half of 64-bit BAR)
                    continue;
                } else {
                    // malformed device: 64-bit BAR but no next BAR - treat as 32-bit
                }
            }

            // 32-bit memory BAR
            uint32 mask = ~(probe & 0xFFFFFFF0);
            uint32 size = mask + 1;
            out->bar[i].is_io = false;
            out->bar[i].size = size;
            out->bar[i].addr = orig & 0xFFFFFFF0;
        }
    }
}

static void pci_enumerate(struct pci_out out[]) {
    pci_device device;
    uint8 bus = 0;
    size out_pos = 0;

    for (uint8 dev = 0; dev < 32; dev++) {
        if (pci_probe_device(bus, dev, 0, &device) != 0) continue;
        out[out_pos++] = (struct pci_out){ .pci = device, .bus = bus, .dev = dev, .func = 0 };

        if ((device.headertype & 0x80) != 0) {
            for (uint8 f = 1; f <= 7; f++) {
                if (pci_probe_device(bus, dev, f, &device) != 0) continue;
                out[out_pos++] = (struct pci_out){ .pci = device, .bus = bus, .dev = dev, .func = f };
            }
        }
    }
}

static struct pci_out pcis[16];

void pci_init(void) {
    pci_enumerate(pcis);
}