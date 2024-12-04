import copy
import json
import os
from typing import Tuple


class BoilerplateBuilder:
    """Create boilerplate code for different the twirl C++ source code."""

    def __init__(
        self,
        comps_using_names=["PairComp", "SceneTriggerComp", "UIComp"],
    ):
        self.comps_using_names = comps_using_names

    def save_scene_to_json(
        self,
        scene_key: str,
        scene_dir_from_nb_dir: str,
        scene_data: dict,
        valid_components: dict,
    ):
        for name, entity_data in scene_data.items():
            # Validate components
            for comp_key, comp_json in copy.deepcopy(entity_data["components"]).items():

                # Validate all input options exist
                for option_key in comp_json.keys():
                    try:
                        assert (
                            option_key in valid_components[comp_key].keys()
                        ), f"Option '{option_key}' not found in {comp_key}"
                    # List format occurs when manual code is used.
                    except AttributeError:
                        # Check both the typical arguments and the manual code
                        assert (option_key in valid_components[comp_key][0].keys()) or (
                            f'"{option_key}"' in valid_components[comp_key][1]
                        ), f"Option '{option_key}' not found in {comp_key}"

                # Add the unresolved name component if the component has unresolved names
                unresolved_comp_included = False
                if (comp_key in self.comps_using_names) and ~unresolved_comp_included:
                    print(f"Adding UnresolvedNameComp to '{name}'")
                    scene_data[name]["components"]["UnresolvedNameComp"] = {}
                    # Only add the UnresolvedNameComp once
                    unresolved_comp_included = True

        with open(
            f"{scene_dir_from_nb_dir}/{scene_key}.json", "w", encoding="utf-8"
        ) as json_file:
            json.dump(scene_data, json_file, indent=4)

    def generate_components_files(
        self, root_dir: str, components_by_file: dict, includes_by_file: dict = {}
    ) -> None:

        # Loop through and generate the header files.
        all_components = {}
        for comp_filename, components in components_by_file.items():
            header_fp = os.path.join(root_dir, f"include/components/{comp_filename}.h")
            self.generate_components_header_file(
                header_fp, components, includes_by_file[comp_filename]
            )
            # We combine all the components into one big dictionary=
            # for use in the source file.
            all_components.update(components)

        self.generate_components_source_file(
            os.path.join(root_dir, "src/components/components.cpp"),
            all_components,
            includes=[],
        )

    def generate_components_header_file(
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
        file_str += f"#ifndef {includeguard_name}\n" f"#define {includeguard_name}\n\n"

        # Add the include statements. nlohmann/json and config.h are always included.
        includes += [
            "<nlohmann/json.hpp>",
            '"config.h"',
        ]
        file_str += "#include " + "\n#include ".join(includes) + "\n\n"

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

    def generate_components_source_file(
        self, save_fp: str, components: dict, includes: list = []
    ) -> None:
        """
        Generates a C++ source file with an emplaceComponent function for an entity-component-system (ECS).

        Args:
            save_fp (str): The file path to save the generated C++ source file.
            components (dict): A dictionary where keys are component names and values
                               are dictionaries of component members.
            includes (list, optional): A list of includes to add to the file.

        Returns:
            None: The generated C++ source file is saved to the specified file path.
        """

        file_str = ""

        # Add the include statements. nlohmann/json and config.h are always included.
        includes += [
            "<entt/entity/fwd.hpp>",
            "<entt/entity/registry.hpp>",
            "<nlohmann/json.hpp>",
            '"components/components.h"',
            '"config.h"',
        ]
        file_str += "#include " + "\n#include ".join(includes) + "\n\n"

        # Add the json shortcut and the namespace
        file_str += (
            "using json = nlohmann::ordered_json;\n\n"
            "namespace twirl {\n\n"
            "namespace comp {\n\n"
        )

        # Add generations for individual functions
        file_str += self.get_emplacecomponent_str(components) + "\n\n"
        file_str += self.get_getentityfromstr_str(components) + "\n\n"
        file_str += self.get_debugentities_str(components) + "\n\n"

        # Wrap up with the namespaces
        file_str += "}  // namespace comp\n" "}  // namespace twirl"

        with open(save_fp, "w", encoding="utf-8") as f:
            f.write(file_str)

    def get_struct_def(
        self, name: str, members: dict = {}, manual_json_code: str = None
    ) -> str:
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

    def get_getentityfromstr_str(self, components: dict) -> str:
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
            self.get_getentityfromstr_str_for_comp(comp_name)
            for comp_name in components.keys()
        ]
        body_str = " else ".join(emplacement_strs)

        # Indent the body string
        body_str = "    " + body_str.replace("\n", "\n    ")

        return (
            "entt::entity getEntityFromStr(entt::registry& registry, const std::string& input_str) {\n"
            "    // Parse the input string\n"
            "    size_t colon = input_str.find('|');\n"
            "    if (colon == std::string::npos) {\n"
            '        throw std::runtime_error("Pipe not found in component string");\n'
            "    }\n"
            "    std::string comp_key = input_str.substr(0, colon);\n"
            "    std::string selection_str = input_str.substr(colon + 1);\n"
            "\n"
            "    // Get the view based on the component string\n"
            + body_str
            + " else {\n"
            '        throw std::runtime_error("Unknown component type");\n'
            "    }\n"
            "}"
        )

    def get_getentityfromstr_str_for_comp(self, name: str) -> str:
        """
        Generates a string representing the emplacement code for a given component.

        Args:
            name (str): The name of the component.
            is_empty (bool, optional): Flag indicating whether the component is empty. Defaults to False.

        Returns:
            str: The generated emplacement string for the component.
        """

        return (
            f'if (comp_key == "{name}") {{\n'
            f"    return getEntityFromSelectionStr<{name}>(registry, selection_str);\n"
            "}"
        )

    def get_debugentities_str(self, components: dict) -> str:

        debug_strs = [
            self.get_debugentity_str_for_comp(comp_name, comp_members)
            for comp_name, comp_members in components.items()
        ]
        body_str = "\n".join(debug_strs)

        # Indent the body string
        body_str = "        " + body_str.replace("\n", "\n        ")

        return (
            "void debugEntities(entt::registry& registry, std::string message) {\n"
            "    auto rview = registry.view<DebugComp>();\n"
            "    bool message_printed = false;\n"
            "    for (auto [entity, debug_c] : rview.each()) {\n"
            "        if (!message_printed) {\n"
            "            std::cout << message << std::endl;\n"
            "            message_printed = true;\n"
            "        }\n"
            '        std::cout << "  entity: " << static_cast<int>(entity) << std::endl;\n'
            "        auto entity_name_ptr = registry.try_get<EntityName>(entity);\n"
            "        if (entity_name_ptr != nullptr) {\n"
            '            std::cout << "  Entity Name: " << *entity_name_ptr << std::endl;\n'
            "\n"
            "        // Skip the rest if not verbose\n"
            "        if (!debug_c.verbose) {\n"
            "            continue;\n"
            "        }\n"
            "\n"
            "        }\n" + body_str + "    }\n"
            "}"
        )

    def get_debugentity_str_for_comp(self, comp_name: str, comp_members: dict) -> str:

        instance_str = comp_name.lower()
        body_str = ""
        for member_name in comp_members.keys():
            body_str += f'    std::cout << "      {member_name}: " << {instance_str}.{member_name} << std::endl;'

        return (
            f"auto {instance_str}_ptr = registry.try_get<{comp_name}>(entity);\n"
            f"if ({instance_str}_ptr != nullptr) {{\n"
            f"    auto {instance_str} = *{instance_str}_ptr;\n"
            'std::cout << "    RenderComp:" << std::endl;\n' + body_str + "}\n"
        )
