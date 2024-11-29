

def get_struct_str(name: str, fields: dict = {}) -> str:
    """Create a struct from a dictionary."""

    if len(fields) == 0:
        return f"struct {name} {{}};"