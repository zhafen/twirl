from typing import Tuple


class BoilerplateBuilder:
    """Create boilerplate code for different the twirl C++ source code."""

    def get_struct_def(self, name: str, members: dict = {}) -> str:

        if len(members) == 0:
            return f"struct {name} {{}};"

        # Add the members
        struct_body_str = ""
        json_body_str = ""
        instance_str = name.lower()
        for member_name, member_args in members.items():

            # Formatting for each member
            if isinstance(member_args, str):
                member_args = [member_args]
            member_str, member_json_str = self.get_member_str(member_name, *member_args)

            # Add the member to the existing struct and json strings
            struct_body_str += "    " + member_str
            if member_json_str is not None:
                json_body_str += f"    {instance_str}.{member_json_str}"

        # Add the struct and json strings to the final output
        struct_str = (
            "struct {name}Comp {{\n" +
            struct_body_str +
            ";}\n"
        )

        # If there's json to load, add that too.
        if json_body_str != "":
            struct_str += (
                f"inline void from_json(const json& j, {name}& {instance_str}) {{\n" +
                json_body_str +
                "}\n"
            )


        return struct_str

    def get_member_str(
        self,
        member_name: str,
        member_type: str,
        json: bool = False,
        default: str = None,
    ) -> Tuple[str]:

        # Get the member string for the struct
        member_str = f"{member_type} {member_name}"
        if default is not None:
            member_str += " = " + str(default)
        member_str += ";\n"

        if not json:
            return member_str, None

        # Get the from_json string
        member_json_str = f"{member_name} = j."
        if default is not None:
            member_json_str += f'value("{member_name}", {default})'
        else:
            member_json_str += f'at("{member_name}").get<{member_type}>()'
        member_json_str += ";\n"

        return member_str, member_json_str
