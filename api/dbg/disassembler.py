import numpy

from dbg.instructions import INSTRUCTION_SET, AddressModes


class Disassembler:
    def __init__(self):
        pass

    @staticmethod
    def disassemble(code: numpy.ndarray) -> dict[int, str]:
        opcodes = {}
        for mnemonic, instruction_set in INSTRUCTION_SET.items():
            for mode, instruction in instruction_set.modes.items():
                entry = {
                    "mnemonic": instruction_set.name,
                    "mode": mode,
                    "size": instruction.size_bytes,
                    "cycles": instruction.cycles,
                }
                if type(instruction.bytecode) == list:
                    for bytecode in instruction.bytecode:
                        opcodes[bytecode] = entry
                else:
                    opcodes[instruction.bytecode] = entry

        instructions = {}
        addr = 0
        while addr < len(code):
            opcode = code[addr]
            if opcode == 0xFF or opcode not in opcodes:
                addr += 1
                continue

            metadata = opcodes[opcode]
            instruction_bytes = code[addr:addr + metadata["size"]]
            addr += metadata["size"]

            argument = Disassembler.argument(
                size=metadata["size"],
                value=int.from_bytes(instruction_bytes[1:], "little"),
                signed=(metadata["mode"] == AddressModes.RELATIVE),
            )

            addressing_mode = Disassembler.format_addressing_mode(metadata["mode"], argument)
            address = addr + 0x8000 + 0x4000
            instructions[address] = f"${address:04x}    {metadata['mnemonic']} {addressing_mode}".strip()

        return instructions

    @staticmethod
    def argument(size: int, value: int, signed: bool) -> str:
        match size:
            case 1:
                return ""
            case 2:
                if signed:
                    signed_value = value if value < 0x80 else value - 0x100
                    return f"{signed_value:+}"
                return f"${value:02x}"
            case 3:
                return f"${value:04x}"

    @staticmethod
    def format_addressing_mode(mode: AddressModes, argument: str) -> str:
        match mode:
            case AddressModes.IMMEDIATE:
                return f"#{argument}"
            case AddressModes.ZEROPAGE:
                return f"{argument}"
            case AddressModes.ZEROPAGE_X:
                return f"{argument},X"
            case AddressModes.ZEROPAGE_Y:
                return f"{argument},Y"
            case AddressModes.ABSOLUTE:
                return f"{argument}"
            case AddressModes.ABSOLUTE_X:
                return f"{argument},X"
            case AddressModes.ABSOLUTE_Y:
                return f"{argument},Y"
            case AddressModes.INDIRECT_X:
                return f"({argument},X)"
            case AddressModes.INDIRECT_Y:
                return f"({argument}),Y"
            case AddressModes.RELATIVE:
                return f"*{argument}"
            case AddressModes.ACCUMULATOR:
                return "A"
            case AddressModes.INDIRECT:
                return f"({argument})"
            case AddressModes.IMPLIED:
                return ""
