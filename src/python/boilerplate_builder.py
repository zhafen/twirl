import os
from typing import Tuple


class BoilerplateBuilder:
    """Create boilerplate code for different the twirl C++ source code."""

    def generate_components_file(
        self, save_fp: str, components: dict, includes: list = []
    ) -> None:
        """
        Generates a C++ header file with struct definitions and an emplaceComponent function.

        Args:
            save_fp (str): The file path to save the generated C++ header file.
            components (dict): A dictionary where keys are component names and values
                               are dictionaries of component members.
            includes (list, optional): A list of includes to add to the file.

        Returns:
            None: The generated C++ header file is saved to the specified file path.
        """


        file_str = ""

        # Add the include guards
        includeguard_name = os.path.basename(save_fp).upper().replace(".", "_")
        file_str += (
            f"#ifndef {includeguard_name}\n"
            f"#define {includeguard_name}\n\n"
        )

        # Add the include statements. nlohmann/json and config.h are always included.
        if len(includes) > 0:
            file_str += "#include " + "\n#include ".join(includes) + "\n"
        file_str += (
            "#include <nlohmann/json.hpp>\n"
            '#include "config.h"\n\n'
        )

        # Add the json shortcut and the namespace
        file_str += "using json = nlohmann::ordered_json;\n\n"
        file_str += "namespace twirl {\n\n"

        # Loop through and add the struct definitions
        for comp_name, comp_args in components.items():
            if isinstance(comp_args, dict):
                comp_args = [comp_args]
            file_str += self.get_struct_def(comp_name, *comp_args) + "\n"

        # Wrap up with the namespace and the include guard
        file_str += "}  // namespace twirl\n\n"
        file_str += f"#endif  // {includeguard_name}"

        with open(save_fp, "w", encoding="utf-8") as f:
            f.write(file_str)

    def get_struct_def(self, name: str, members: dict = {}, manual_json_code: str = None) -> str:
        """
        Generates a C++ struct definition and an optional JSON deserialization function.

        Args:
            name (str): The name of the struct.
            members (dict, optional): A dictionary where keys are member names and values are either
                                      a single type as a string or a list of arguments for the member.
                                      Defaults to an empty dictionary.
            manual_json_code (str, optional): A string of manual JSON deserialization code. Defaults to None.

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

        if manual_json_code is not None:
            # Assume the manual_json_code is not indented, so we indent it here.
            json_body_str += "    " + manual_json_code.replace("\n", "\n    ")[:-4]

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
        is_serializable: bool = False,
        default: str = None,
        scale: str = None,
    ) -> Tuple[str]:
        """
        Generates a member string for a struct def and optionally a from_json method string.

        Args:
            member_name (str): The name of the member.
            member_type (str): The type of the member.
            is_serializable (bool, optional): If True, also generate a string for the from_json method.
            default (str, optional): The default value for the member. Defaults to None.
            scale (str, optional): The scale factor for the member. Defaults to None.

        Returns:
            Tuple[str]: A tuple containing the member string and optionally the JSON parsing string.
        """

        # Get the member string for the struct
        member_str = f"{member_type} {member_name}"
        if default is not None:
            member_str += " = " + str(default)
            if scale is not None:
                member_str += f" * {scale}"
        member_str += ";\n"

        if not is_serializable:
            return member_str, None

        # Get the from_json string
        member_json_str = f"{member_name} = j."
        if default is not None:
            member_json_str += f'value("{member_name}", {default})'
            if scale is not None:
                member_json_str += f" * {scale}"
        else:
            member_json_str += f'at("{member_name}").get<{member_type}>()'
        member_json_str += ";\n"

        return member_str, member_json_str

    def get_emplacecomponent_str(self, components: dict) -> str:
        """
        Generates a string representing the function definition for emplacing components
        in an entity-component-system (ECS) registry.

        Args:
            components (dict): A dictionary where keys are component names and values
                               are lists of component members.

        Returns:
            str: A string representing the function definition for emplacing components
                 in the ECS registry.
        """

        emplacement_strs = [
            self.get_emplacement_str_for_comp(comp_name, len(comp_members) == 0)
            for comp_name, comp_members in components.items()
        ]
        body_str = " else ".join(emplacement_strs)

        # Indent the body string
        body_str = "    " + body_str.replace("\n", "\n    ")

        return (
            "void emplaceComponent(entt::registry& registry, entt::entity entity,\n"
            "                      const std::string& comp_key, const json& comp_json) {\n"
            + body_str
            + " else {\n"
            '        throw std::runtime_error("Unknown component type");\n'
            "    }\n"
            "}"
        )

    def get_emplacement_str_for_comp(self, name: str, is_empty: bool = False) -> str:
        """
        Generates a string representing the emplacement code for a given component.

        Args:
            name (str): The name of the component.
            is_empty (bool, optional): Flag indicating whether the component is empty. Defaults to False.

        Returns:
            str: The generated emplacement string for the component.
        """

        if is_empty:
            return (
                f'if (comp_key == "{name}") {{\n'
                f"    registry.emplace<{name}>(entity);\n"
                "}"
            )

        return (
            f'if (comp_key == "{name}") {{\n'
            f"    auto {name.lower()} = comp_json.template get<{name}>();\n"
            f"    registry.emplace<{name}>(entity, {name.lower()});\n"
            "}"
        )
