#pragma once

#include "types.h"
#include "port.hpp"
#include "gdt.hpp"

class InterruptManager
{
protected:
	struct GateDescriptor
	{
		uint16_t handler_address_low_bits;
		uint16_t gdt_code_segment_selector;
		uint8_t reserved;
		uint8_t access;
		uint16_t handler_address_high_bits;
		
	} __attribute__((packed));
	
	static GateDescriptor interrupt_descriptor_table[256];
	
	struct InterruptDescriptorTablePointer
	{
		uint16_t size;
		uint32_t base;
		
	} __attribute__((packed));
	
	static void setInterruptDescriptorTableEntry(
		uint8_t interrupt_number,
		uint16_t gdt_code_segment_selector_offset,
		void (*handler)(),
		uint8_t descriptor_privelege_level,
		uint8_t descriptor_type
	);
	
	Port8BitSlow pic_master_command;
	Port8BitSlow pic_master_data;
	Port8BitSlow pic_slave_command;
	Port8BitSlow pic_slave_data;
	
public:
	InterruptManager(GlobalDescriptorTable *gdt);
	~InterruptManager();
	
	void activate();

	static uint32_t handleInterrupt(uint8_t interrupt_number, uint32_t esp);
	
	static void IgnoreInterruptRequest();
	static void HandleInterruptRequest0x00();
	static void HandleInterruptRequest0x01();
};
