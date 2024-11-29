from typing import Tuple


class BoilerplateBuilder:
    """Create boilerplate code for different the twirl C++ source code."""

    def get_struct_def(self, name: str, members: dict = {}) -> str:
        """
        Generates a C++ struct definition and an optional JSON deserialization function.

        Args:
            name (str): The name of the struct.
            members (dict, optional): A dictionary where keys are member names and values are either
                                      a single type as a string or a list of arguments for the member.
                                      Defaults to an empty dictionary.

        Returns:
            str: The C++ struct definition as a string, including member variables and an optional
                 JSON deserialization function if members are provided.
        """

        if len(members) == 0:
            return f"struct {name} {{}};\n"

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
        struct_str = f"struct {name} {{\n" + struct_body_str + "};\n"

        # If there's json to load, add that too.
        if json_body_str != "":
            struct_str += (
                f"inline void from_json(const json& j, {name}& {instance_str}) {{\n"
                + json_body_str
                + "}\n"
            )

        return struct_str

    def get_member_str(
        self,
        member_name: str,
        member_type: str,
        json: bool = False,
        default: str = None,
    ) -> Tuple[str]:
        """
        Generates a member string for a struct def and optionally a from_json method string.

        Args:
            member_name (str): The name of the member.
            member_type (str): The type of the member.
            json (bool, optional): If True, also generate a string for the from_json method.
            default (str, optional): The default value for the member. Defaults to None.

        Returns:
            Tuple[str]: A tuple containing the member string and optionally the JSON parsing string.
        """

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

    def get_emplacecomponent_str(self, components: dict) -> str:

        emplacement_strs = [
            self.get_emplacement_str_for_comp(comp_name, len(comp_members) == 0)
            for comp_name, comp_members in components.items()
        ]
        body_str = " else ".join(emplacement_strs)

    def get_emplacement_str_for_comp(self, name: str, is_empty: bool = False) -> str:

        if is_empty:
            return (
                f'(comp_key == "{name}") {{\n'
                f"    registry.emplace<{name}>(entity);\n"
                "}"
            )

        return (
            f'(comp_key == "{name}") {{\n'
            f"    auto {name.lower()} = comp_json.template get<{name}>();\n"
            f"    registry.emplace<{name}>(entity, {name.lower()});\n"
            "}"
        )
