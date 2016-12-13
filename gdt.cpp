#include "gdt.hpp"

GlobalDescriptorTable::GlobalDescriptorTable() :
	null_segment_selector(0, 0, 0),
	unused_segment_selector(0, 0, 0),
	code_segment_selector(0, 64*1024*1024, 0x9A),	// 64MB
	data_segment_selector(0, 64*1024*1024, 0x92)	// 64MB
{
	// Construct the table
	uint32_t i[2];
	i[0] = (uint32_t)this;							// Low bytes
	i[1] = sizeof(GlobalDescriptorTable) << 16;		// High bytes
	
	// Tells the processor to use this GDT
	asm volatile("lgdt (%0)": :"p" (((uint8_t *)i)+2));
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
	// TODO: Should unload GDT
}

uint16_t GlobalDescriptorTable::codeSegmentSelector()
{
	// Return the address fo the selector - the address of the table
	return (uint8_t *)&code_segment_selector - (uint8_t *)this;
}

uint16_t GlobalDescriptorTable::dataSegmentSelector()
{
	// Return the address of the selector - the address of the table
	return (uint8_t *)&data_segment_selector - (uint8_t *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base,
															uint32_t limit,
															uint8_t flags)
{
	uint8_t *target = (uint8_t *)this;
	
	// 16-bit limit
	if (limit <= 65536)
	{
		// Set the 6th byte - tells processor it's a 16-bit entry
		target[6] = 0x40;
	}
	else
	{
		// If the last 12 bits of the limit are not all 1
		if ((limit & 0xFFF) != 0xFFF)
			limit = (limit >> 12) - 1;
		else
			limit = limit >> 12;
			
		target[6] = 0xC0;
	}
	
	target[0] = limit & 0xFF;
	target[1] = (limit >> 8) & 0xFF;
	target[6] |= (limit >> 16) & 0xF;
	
	target[2] = base & 0xFF;
	target[3] = (base >> 8) & 0xFF;
	target[4] = (base >> 16) & 0xFF;
	target[7] = (base >> 24) & 0xFF;
	
	target[5] = flags;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::base()
{
	uint8_t *target = (uint8_t *)this;
	uint32_t result = target[7];
	result = (result << 8) + target[4];
	result = (result << 8) + target[3];
	result = (result << 8) + target[2];
	return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::limit()
{
	uint8_t *target = (uint8_t *)this;
	uint32_t result = target[6] & 0xF;
	result = (result << 8) + target[1];
	result = (result << 8) + target[0];
	
	if ((target[6] & 0xC0) == 0xC0)
		result = (result << 12) | 0xFFF;
	
	return result;
}
