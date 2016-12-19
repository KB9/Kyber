#include "port.hpp"

Port::Port(uint16_t port_number)
{
	this->port_number = port_number;
}

Port::~Port()
{
	
}

// 8-bit port

Port8Bit::Port8Bit(uint16_t port_number) : Port(port_number)
{
	
}

Port8Bit::~Port8Bit()
{
	
}

void Port8Bit::write(uint8_t data)
{
	__asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port_number));
}

uint8_t Port8Bit::read()
{
	uint8_t result;
	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port_number));
	return result;
}

// 8-bit slow port

Port8BitSlow::Port8BitSlow(uint16_t port_number) : Port8Bit(port_number)
{
	
}

Port8BitSlow::~Port8BitSlow()
{
	
}

void Port8BitSlow::write(uint8_t data)
{
	__asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port_number));
}

// 16-bit port

Port16Bit::Port16Bit(uint16_t port_number) : Port(port_number)
{
	
}

Port16Bit::~Port16Bit()
{
	
}

void Port16Bit::write(uint16_t data)
{
	__asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port_number));
}

uint16_t Port16Bit::read()
{
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port_number));
	return result;
}

// 32-bit port

Port32Bit::Port32Bit(uint16_t port_number) : Port(port_number)
{
	
}

Port32Bit::~Port32Bit()
{
	
}

void Port32Bit::write(uint32_t data)
{
	__asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (port_number));
}

uint32_t Port32Bit::read()
{
	uint32_t result;
	__asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port_number));
	return result;
}
