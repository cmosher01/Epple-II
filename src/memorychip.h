#ifndef MEMORYCHIP_H
#define MEMORYCHIP_H

#include <cstdint>
#include <vector>
#include <string>

class MemoryChip {
    private:
        const std::uint16_t size_bits;
        const std::string id_model;
    public:
        static MemoryChip *instance(const std::string &id_model);
        MemoryChip(const std::uint16_t size, const std::string &id_model);
        virtual ~MemoryChip();
        std::uint16_t size() const;
        std::string id() const;
        virtual void init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const;
        virtual bool exists() const;
    protected:
        virtual void rand_init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size, const std::uint_fast32_t cycle, const double glitch, const double chaos) const;
};

class MemoryChipEmptySocket : public MemoryChip {
    public:
        MemoryChipEmptySocket();
        virtual ~MemoryChipEmptySocket();
        virtual bool exists() const;
};

class MemoryChipMM5290 : public MemoryChip {
    public:
        MemoryChipMM5290();
        virtual ~MemoryChipMM5290();
        virtual void init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const;
};

class MemoryChipMK4116 : public MemoryChip {
    public:
        MemoryChipMK4116();
        virtual ~MemoryChipMK4116();
        virtual void init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const;
};

class MemoryChipMCM4116 : public MemoryChip {
    public:
        MemoryChipMCM4116();
        virtual ~MemoryChipMCM4116();
        virtual void init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const;
};

#endif // MEMORYCHIP_H
