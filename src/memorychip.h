#ifndef MEMORYCHIP_H
#define MEMORYCHIP_H

#include <cstdint>
#include <string>

class MemoryChip {
    private:
        std::uint16_t size_bits;
        std::string id_model;
    public:
        MemoryChip(const std::uint16_t size, const std::string &id_model);
        MemoryChip(const MemoryChip &that);
        MemoryChip &operator=(const MemoryChip &that);
        virtual ~MemoryChip();
        std::uint16_t size() const;
        std::string id() const;
        virtual void init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const;
        virtual bool exists() const;
};

class MemoryChipEmptySocket : public MemoryChip {
    public:
        MemoryChipEmptySocket();
        virtual ~MemoryChipEmptySocket();
        virtual bool exists() const;
};

#endif // MEMORYCHIP_H
