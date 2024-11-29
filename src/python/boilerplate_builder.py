

class BoilerplateBuilder:
    """Create boilerplate code for different the twirl C++ source code."""

    def get_struct_str(self, name: str, fields: dict = {}) -> str:

        if len(fields) == 0:
            return f"struct {name} {{}};"

        struct_str = f"struct {name} {{\n"
        for field, field_type in fields.items():
            struct_str += f"    {field_type} {field};\n"
        struct_str += "};"

        return struct_str
