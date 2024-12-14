import os

import unittest
from src.python.boilerplate_builder import BoilerplateBuilder


class TestBoilerplateBuilder(unittest.TestCase):

    def setUp(self):
        self.builder = BoilerplateBuilder()
        self.output_header_fp = "tests/test_python/test_generated_components.h.test"
        self.output_source_fp = "tests/test_python/test_generated_components.cpp.test"

    def tearDown(self):
        if os.path.exists(self.output_header_fp):
            os.remove(self.output_header_fp)
        if os.path.exists(self.output_source_fp):
            os.remove(self.output_source_fp)

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
                "scene_name": ["std::string", True],
                "scene_entity": ["entt::entity", False, "entt::null"],
                "n_triggers": ["int", True, 0],
            },
        )

        assert expected_output == output

    def test_get_struct_w_custom_from_json(self):

        expected_output = (
            "struct CircleComp {\n"
            "    CenteredCircleShape shape;\n"
            "    int zorder = 0;\n"
            "};\n"
            "inline void from_json(const json& j, CircleComp& CircleComp) {\n"
            '    CircleComp.zorder = j.value("zorder", 0);\n'
            '    auto radius = j.value("radius", 1.0f) * cfg.L;\n'
            '    auto outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;\n'
            '    auto fill_color = j.value("fill_color", sf::Color::White);\n'
            '    auto outline_color = j.value("outline_color", sf::Color::Black);\n'
            "    CircleComp.shape = CenteredCircleShape(radius);\n"
            "    CircleComp.shape.setOutlineThickness(outline_thickness);\n"
            "    CircleComp.shape.setFillColor(fill_color);\n"
            "    CircleComp.shape.setOutlineColor(outline_color);\n"
            "}\n"
        )

        output = self.builder.get_struct_def(
            "CircleComp",
            {
                "shape": "CenteredCircleShape",
                "zorder": ["int", True, "0"],
            },
            manual_json_code=(
                'auto radius = j.value("radius", 1.0f) * cfg.L;\n'
                'auto outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;\n'
                'auto fill_color = j.value("fill_color", sf::Color::White);\n'
                'auto outline_color = j.value("outline_color", sf::Color::Black);\n'
                "CircleComp.shape = CenteredCircleShape(radius);\n"
                "CircleComp.shape.setOutlineThickness(outline_thickness);\n"
                "CircleComp.shape.setFillColor(fill_color);\n"
                "CircleComp.shape.setOutlineColor(outline_color);\n"
            ),
        )

        assert expected_output == output

    def test_get_emplacecomponent_str(self):
        expected_output = (
            "void emplaceComponent(entt::registry& registry, entt::entity entity,\n"
            "                      const std::string& comp_key, const json& comp_json) {\n"
            '    if (comp_key == "ViewFlag") {\n'
            "        registry.emplace<ViewFlag>(entity);\n"
            '    } else if (comp_key == "ValueBarComp") {\n'
            "        auto valuebarcomp = comp_json.template get<ValueBarComp>();\n"
            "        registry.emplace<ValueBarComp>(entity, valuebarcomp);\n"
            "    } else {\n"
            '        throw std::runtime_error("Unknown component type: " + comp_key);\n'
            "    }\n"
            "}"
        )
        output = self.builder.get_emplacecomponent_str(
            {
                "ViewFlag": [],
                "ValueBarComp": ["ValueBarComp"],
            }
        )
        assert expected_output == output

    def test_get_emplacement_str_for_comp(self):

        expected_output_empty = (
            'if (comp_key == "ViewFlag") {\n'
            "    registry.emplace<ViewFlag>(entity);\n"
            "}"
        )
        output_empty = self.builder.get_emplacement_str_for_comp("ViewFlag", True)
        assert expected_output_empty == output_empty

        expected_output = (
            'if (comp_key == "ValueBarComp") {\n'
            "    auto valuebarcomp = comp_json.template get<ValueBarComp>();\n"
            "    registry.emplace<ValueBarComp>(entity, valuebarcomp);\n"
            "}"
        )
        output = self.builder.get_emplacement_str_for_comp("ValueBarComp")
        assert expected_output == output

    def test_generate_components_header_file(self):

        components = {
            "EnemyFlag": {},
            "SceneComp": {
                "scene_fp": ["std::string", True],
                "emplace_after_loading": ["bool", True, "false"],
                "json_data": "json",
                "n_emplaced": ["size_t", False, "0"],
            },
            "PhysicsComp": {
                "mass": ["float", True, "1.0f"],
                "pos": ["sf::Vector2f", True, "sf::Vector2f(0.0f, 0.0f)", "cfg.L"],
                "vel": ["sf::Vector2f", True, "sf::Vector2f(0.0f, 0.0f)", "cfg.V"],
                "force": ["sf::Vector2f", False, "sf::Vector2f(0.0f, 0.0f)", "cfg.A"],
                "collided": ["bool", False, "false"],
            },
            "CircleComp": [
                {
                    "shape": "CenteredCircleShape",
                    "zorder": ["int", True, "0"],
                },
                (
                    'auto radius = j.value("radius", 1.0f) * cfg.L;\n'
                    'auto outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;\n'
                    'auto fill_color = j.value("fill_color", sf::Color::White);\n'
                    'auto outline_color = j.value("outline_color", sf::Color::Black);\n'
                    "CircleComp.shape = CenteredCircleShape(radius);\n"
                    "CircleComp.shape.setOutlineThickness(outline_thickness);\n"
                    "CircleComp.shape.setFillColor(fill_color);\n"
                    "CircleComp.shape.setOutlineColor(outline_color);\n"
                ),
            ],
        }

        self.builder.generate_components_header_file(
            self.output_header_fp,
            components,
            includes=["<string>", "<SFML/Graphics.hpp>"],
        )

        with open(
            "tests/test_python/test_expected_components.h.test", "r", encoding="utf-8"
        ) as file:
            expected_output = file.read()
        with open(self.output_header_fp, "r", encoding="utf-8") as file:
            output = file.read()

        assert expected_output == output

    def test_generate_components_source_file(self):

        components = {
            "EnemyFlag": {},
            "PhysicsComp": {
                "mass": ["float", True, "1.0f"],
                "pos": ["sf::Vector2f", True, "sf::Vector2f(0.0f, 0.0f)", "cfg.L"],
                "vel": ["sf::Vector2f", True, "sf::Vector2f(0.0f, 0.0f)", "cfg.V"],
                "force": ["sf::Vector2f", False, "sf::Vector2f(0.0f, 0.0f)", "cfg.A"],
                "collided": ["bool", False, "false"],
            },
            "CircleComp": [
                {
                    "shape": "CenteredCircleShape",
                    "zorder": ["int", True, "0"],
                },
                (
                    'auto radius = j.value("radius", 1.0f) * cfg.L;\n'
                    'auto outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;\n'
                    'auto fill_color = j.value("fill_color", sf::Color::White);\n'
                    'auto outline_color = j.value("outline_color", sf::Color::Black);\n'
                    "CircleComp.shape = CenteredCircleShape(radius);\n"
                    "CircleComp.shape.setOutlineThickness(outline_thickness);\n"
                    "CircleComp.shape.setFillColor(fill_color);\n"
                    "CircleComp.shape.setOutlineColor(outline_color);\n"
                ),
            ],
        }

        self.builder.generate_components_source_file(
            self.output_source_fp,
            components,
        )

        with open(
            "tests/test_python/test_expected_components.cpp.test", "r", encoding="utf-8"
        ) as file:
            expected_output = file.read()
        with open(self.output_source_fp, "r", encoding="utf-8") as file:
            output = file.read()

        assert expected_output == output