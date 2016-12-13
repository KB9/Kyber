#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

// Data structure used by Intel x86-family processors in order to define the
// characteristics of various memory areas during program execution.
// Memory areas include the base address, the size and access priveleges like
// executability and writability.
// These memory areas are called segments.
class GlobalDescriptorTable
{
public:
	
	// Part of the segmentation unit, used for translating a logical address to
	// a linear address. A segment descriptor contains:
	// - A segment base address
	// - The segment limit which specifies the segment size
	// - Access rights byte containing the protection mechanism information
	// - Control bits
	class SegmentDescriptor
	{
	public:
		SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
		uint32_t base();
		uint32_t limit();
		
	private:
		uint16_t limit_lo;		// Limit low bytes
		uint16_t base_lo;		// Low bytes of the pointer
		uint8_t base_hi;		// 1 byte extension for the pointer
		uint8_t type;			// Acccess byte
		uint8_t flags_limit_hi;
		uint8_t base_vhi;
		
	} __attribute__((packed));
	
	SegmentDescriptor null_segment_selector;
	SegmentDescriptor unused_segment_selector;
	SegmentDescriptor code_segment_selector;
	SegmentDescriptor data_segment_selector;
	
	GlobalDescriptorTable();
	~GlobalDescriptorTable();
	
	uint16_t codeSegmentSelector();
	uint16_t dataSegmentSelector();
};
