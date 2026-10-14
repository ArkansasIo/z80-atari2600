#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
namespace z80 {
struct Registers { std::uint8_t a{}, f{}, b{}, c{}, d{}, e{}, h{}, l{}; std::uint16_t sp{0xFFFF}, pc{}; };
struct StepResult { unsigned cycles{}; bool halted{}; };
class Cpu {
public:
 static constexpr std::size_t memory_size = 65536;
 Registers r{};
 std::array<std::uint8_t, memory_size> memory{};
 bool halted{false};
 void reset();
 void load(const std::vector<std::uint8_t>& program, std::uint16_t address = 0);
 StepResult step();
 void run(std::uint64_t instruction_limit = 1'000'000);
private:
 std::uint8_t fetch8();
 std::uint16_t fetch16();
 void set_zero_sign(std::uint8_t value);
};
}
