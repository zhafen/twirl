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
