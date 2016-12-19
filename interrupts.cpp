#include "interrupts.hpp"

void kprint(char *text);

InterruptManager::GateDescriptor InterruptManager::interrupt_descriptor_table[256];

void InterruptManager::setInterruptDescriptorTableEntry(
	uint8_t interrupt_number,
	uint16_t gdt_code_segment_selector_offset,
	void (*handler)(),
	uint8_t descriptor_privelege_level,
	uint8_t descriptor_type)
{
	const uint8_t IDT_DESC_PRESENT = 0x80;

	interrupt_descriptor_table[interrupt_number].handler_address_low_bits = ((uint32_t)handler) & 0xFFFF;
	interrupt_descriptor_table[interrupt_number].handler_address_high_bits = (((uint32_t)handler) >> 16) & 0xFFFF; 
	interrupt_descriptor_table[interrupt_number].gdt_code_segment_selector = gdt_code_segment_selector_offset;
	interrupt_descriptor_table[interrupt_number].access = IDT_DESC_PRESENT | descriptor_type | ((descriptor_privelege_level & 3) << 5);
	interrupt_descriptor_table[interrupt_number].reserved = 0;
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt) :
	pic_master_command(0x20),
	pic_master_data(0x21),
	pic_slave_command(0xA0),
	pic_slave_data(0xA1)
{
	uint16_t code_segment = gdt->codeSegmentSelector();
	const uint8_t IDT_INTERRUPT_GATE = 0xE;
	
	for (uint16_t i = 0; i < 256; i++)
	{
		setInterruptDescriptorTableEntry(i, code_segment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
	}
	
	// 0x0 + IRQ_BASE (0x20) = 0x20 (from interruptstubs.s)
	setInterruptDescriptorTableEntry(0x20, code_segment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
	setInterruptDescriptorTableEntry(0x21, code_segment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
	
	pic_master_command.write(0x11);
	pic_slave_command.write(0x11);
	
	pic_master_data.write(0x20);
	pic_slave_data.write(0x28);
	
	pic_master_data.write(0x04);
	pic_slave_data.write(0x02);
	
	pic_master_data.write(0x01);
	pic_slave_data.write(0x01);
	
	pic_master_data.write(0x00);
	pic_slave_data.write(0x00);
	
	InterruptDescriptorTablePointer idt;
	idt.size = 256 * sizeof(GateDescriptor) - 1;
	idt.base = (uint32_t)interrupt_descriptor_table;
	asm volatile("lidt %0" : : "m" (idt));
	
	kprint("InterruptManager initialized...");
}

InterruptManager::~InterruptManager()
{
	
}

void InterruptManager::activate()
{
	asm("sti");
	
	kprint("InterruptManager activated...");
}

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
	kprint("InterruptManager handling interrupt...");
	return esp;
}
