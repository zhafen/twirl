import unittest
from src.python.boilerplate_builder import BoilerplateBuilder


class TestBoilerplateBuilder(unittest.TestCase):

    def setUp(self):
        self.builder = BoilerplateBuilder()

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

    def test_get_emplacement_str(self):

        expected_output_empty = (
            '(comp_key == "ViewComp") {\n'
            "    registry.emplace<ViewComp>(entity);\n"
            "}\n"
        )
        output_empty = self.builder.get_emplacement_str("ViewComp")
        assert expected_output_empty == output_empty

        expected_output = (
            '(comp_key == "UIComp") {\n'
            "    auto comp_inst = comp_json.template get<UIComp>();\n"
            "    registry.emplace<UIComp>(entity, comp_inst);\n"
            "}\n"
        )
        output = self.builder.get_emplacement_str("UIComp", True)
        assert expected_output == output
