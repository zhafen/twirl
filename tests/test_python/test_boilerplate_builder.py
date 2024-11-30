import unittest
from src.python.boilerplate_builder import BoilerplateBuilder
import os


class TestBoilerplateBuilder(unittest.TestCase):

    def setUp(self):
        self.builder = BoilerplateBuilder()
        self.generated_components_fp = "tests/test_python/test_generated_components.h"

    def tearDown(self):

        if os.path.exists(self.generated_components_fp):
            os.remove(self.generated_components_fp)

    def test_get_struct_str_empty_members(self):
        name = "MyStruct"
        members = {}
        expected_output = "struct MyStruct {};\n"
        self.assertEqual(self.builder.get_struct_def(name, members), expected_output)

    def test_get_struct_str_single_member(self):
        name = "MyStruct"
        members = {"member1": "int"}
        expected_output = "struct MyStruct {\n    int member1;\n};\n"
        self.assertEqual(self.builder.get_struct_def(name, members), expected_output)

    def test_get_struct_str_multiple_members(self):
        name = "MyStruct"
        members = {"member1": "int", "member2": "float", "member3": "std::string"}
        expected_output = (
            "struct MyStruct {\n"
            "    int member1;\n"
            "    float member2;\n"
            "    std::string member3;\n"
            "};\n"
        )
        self.assertEqual(self.builder.get_struct_def(name, members), expected_output)

    def test_get_struct_w_from_json(self):

        expected_output = (
            "struct SceneTriggerComp {\n"
            "    std::string scene_name;\n"
            "    entt::entity scene_entity = entt::null;\n"
            "    int n_triggers = 0;\n"
            "};\n"
            "inline void from_json(const json& j, SceneTriggerComp& scenetriggercomp) {\n"
            '    scenetriggercomp.scene_name = j.at("scene_name").get<std::string>();\n'
            '    scenetriggercomp.n_triggers = j.value("n_triggers", 0);\n'
            "}\n"
        )

        output = self.builder.get_struct_def(
            "SceneTriggerComp",
            {
                "scene_name": [
                    "std::string",
                    True,
                ],
                "scene_entity": ["entt::entity", False, "entt::null"],
                "n_triggers": ["int", True, 0],
            },
        )

        assert expected_output == output

    def test_get_emplacecomponent_str(self):
        expected_output = (
            "void emplaceComponent(entt::registry& registry, entt::entity entity,\n"
            "                      const std::string& comp_key, const json& comp_json) {\n"
            '    if (comp_key == "ViewComp") {\n'
            "        registry.emplace<ViewComp>(entity);\n"
            '    } else if (comp_key == "UIComp") {\n'
            "        auto uicomp = comp_json.template get<UIComp>();\n"
            "        registry.emplace<UIComp>(entity, uicomp);\n"
            "    } else {\n"
            '        throw std::runtime_error("Unknown component type");\n'
            "    }\n"
            "}"
        )
        output = self.builder.get_emplacecomponent_str(
            {
                "ViewComp": [],
                "UIComp": ["uicomp"],
            }
        )
        assert expected_output == output

    def test_get_emplacement_str_for_comp(self):

        expected_output_empty = (
            'if (comp_key == "ViewComp") {\n'
            "    registry.emplace<ViewComp>(entity);\n"
            "}"
        )
        output_empty = self.builder.get_emplacement_str_for_comp("ViewComp", True)
        assert expected_output_empty == output_empty

        expected_output = (
            'if (comp_key == "UIComp") {\n'
            "    auto uicomp = comp_json.template get<UIComp>();\n"
            "    registry.emplace<UIComp>(entity, uicomp);\n"
            "}"
        )
        output = self.builder.get_emplacement_str_for_comp("UIComp")
        assert expected_output == output

    def test_generate_components_file(self):

        components = {
            "EnemyComp": {},
            "SceneComp": {
                "scene_fp": ["std::string", True],
                "emplace_after_loading": ["bool", True, "false"],
                "json_data": "json",
                "n_emplaced": ["size_t", False, "0"],
                "verbose_names": ["bool", True, "true"],
            },
            "PhysicsComp": {
                "mass": ["float", True, "1.0f"],
                "pos": ["sf::Vector2f", True, "sf::Vector2f(0.0f, 0.0f)", "cfg.L"],
                "vel": ["sf::Vector2f", True, "sf::Vector2f(0.0f, 0.0f)", "cfg.V"],
                "force": ["sf::Vector2f", False, "sf::Vector2f(0.0f, 0.0f)", "cfg.A"],
                "collided": ["bool", False, "false"],
            },
        }

        self.builder.generate_components_file(
            self.generated_components_fp,
            components,
            includes=["<string>", "<SFML/Graphics.hpp>"],
        )

        with open(
            "tests/test_python/test_expected_components.h", "r", encoding="utf-8"
        ) as file:
            expected_output = file.read()
        with open(self.generated_components_fp, "r", encoding="utf-8") as file:
            output = file.read()

        assert expected_output == output
