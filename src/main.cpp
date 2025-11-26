/**
 * For students:
 * To complete the assignment, you have only two lines to pay attention to in
 * this file.
 */
#include <fstream>

#include "nlohmann/json.hpp"
#include "rdr/interaction.h"
#include "rdr/render.h"

using namespace RDR_NAMESPACE_NAME;

static void printHelp(int, char *argv[]) {  // NOLINT
  std::ostringstream oss;
  oss << "RDR171 version 0.1, Copyright (c) ShanghaiTech CS171 TAs\n"
      << "Please DO NOT EVER release the source code containing your "
         "implementations\n"
      << format("Usage: {} [options] <One scene JSON file>\n", argv[0])
      << format("  --help,-h             Print this help text.\n")
      << format("  --quite,-q            Not output during rendering.\n")
      << format("  --output,-o <path>    Override the default output path.\n")
      << format(
             "  --override  <json>    Override the scene specification with a "
             "single-line json,\n"
             "                        e.g. --override "
             "'{{\"integrator\":{{\"type\":\"path\",\"profile\":\"MIS\"}}}}'"
             "\n");
  print("{}", oss.str());
}

int rdr_main(int argc, char *argv[]) {  // NOLINT: alias of main function
  bool quiet = false;
  fs::path source_path{};
  std::optional<std::string> output_path{};
  std::optional<std::string> override_json_string{};

  if (argc <= 1) {
    printHelp(argc, argv);
    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      printHelp(argc, argv);
      return 0;
    } else if (arg == "--quite" || arg == "-q") {
      quiet = true;
    } else if (arg == "--output" || arg == "-o") {
      if (i + 1 < argc) {
        output_path = argv[++i];
      } else {
        print("Missing output path after [ {} ]\n", arg);
        printHelp(argc, argv);
        return 1;
      }
    } else if (arg == "--override") {
      if (i + 1 < argc) {
        override_json_string = argv[++i];
      } else {
        print("Missing JSON override after [ {} ]\n", arg);
        printHelp(argc, argv);
        return 1;
      }
    } else {
      source_path = arg;
    }
  }

  // Init logger first anyway
  InitLogger(true, quiet);

  // Register factory classes
  Factory::doRegisterAllClasses();
  print("//=== RDR171 Renderer ===//\n");
  Info_("===    RDR171 Launching    ===");
  Info_("===    HAPPY RENDERING!    ===");

  if (source_path.extension() != ".json") {
    Exception_("Please specify a JSON file as the scene configuration");
    printHelp(argc, argv);
    return 1;
  }

  // Check if the file exists
  if (!fs::exists(source_path)) {
    Exception_("File {} does not exist", source_path.string());
    return 1;
  }

  // Initialize file resolver
  FileResolver::setBasePath(source_path.parent_path());

  Info_("FileResolver has been initialized with base path [ {} ]",
      source_path.parent_path().string());

  /// Load config from json file
  std::ifstream fin;
  fin.open(source_path);

  if (!fin.is_open()) {
    Exception_("Can not open the JSON file [ {} ]", source_path.string());
    return 1;
  } else {
    Info_("JSON file loaded from [ {} ]", source_path.string());
  }

  print("// Parsing scene from JSON file...\n");
  // Parse json object to Config
  nlohmann::json root_json;
  Properties root_properties;
  try {
    fin >> root_json;
    if (override_json_string.has_value())
      root_json.update(
          nlohmann::json::parse(override_json_string.value()), true);
    root_properties = Properties(root_json);
  } catch (nlohmann::json::exception &ex) {
    // print error message
    print("JSON parsing error: {}\n", ex.what());
    Exception_("{}", ex.what());
    return 1;
  }

  if (!output_path.has_value())
    output_path = source_path.filename().stem().string() + ".exr";
  Info_("Root Properties initialized with [ JSON ]. Start building scene...");
  ref<RenderInterface> render = make_ref<NativeRender>(root_properties);

  print("// Building scene...\n");
  render->initialize();
  render->preprocess();

  /*===---------------------------------------------------------------===*
   // Start rendering
   *===---------------------------------------------------------------===*/
  print("Starting rendering...\n");
  Info_("Scene built. Start rendering...");
  auto start = std::chrono::steady_clock::now();

  render->render();
  render->exportImageToDisk(output_path.value());

  auto end = std::chrono::steady_clock::now();
  auto time =
      std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

  Info_("Render Finished in {}s", time);
  print("=== RDR171 Finished ===\n");
  return 0;
  // Question: Info_是不是输出了什么？我在我的终端里没有找到相关的输出。
  // Answer:
  // 是的，Info_是一个日志输出宏，用于在渲染器运行过程中输出信息性日志。它通常用于记录程序的执行状态、重要事件或调试信息，帮助开发者和用户了解渲染过程中的各个步骤和状态变化。如果您在终端中没有看到相关输出，可能是因为日志级别设置较高，或者输出被重定向到了其他地方。您可以检查日志配置，确保Info级别的日志被正确显示在终端中。
  // Question: 我应该去哪查看日志设置？
  // Answer:
  // 日志设置通常在渲染器的初始化代码中进行配置。在这个渲染器中，日志初始化是在InitLogger函数中完成的。您可以查看该函数的实现，了解日志的输出目标和日志级别的设置。此外，您还可以检查spdlog库的文档，因为这个渲染器使用了spdlog作为日志库。确保在调用InitLogger时传递了正确的参数，以便启用所需级别的日志输出。如果需要，您还可以修改InitLogger函数，以更改日志的输出位置（例如，输出到文件而不是终端）或调整日志级别。
}

int main(int argc, char *argv[]) {
  int ret_val = 0;
  print("=== RDR171 Starting ===\n");

  try {
    ret_val = rdr_main(argc, argv);
  } catch (const rdr_exception &e) {
    ret_val = 1;
    Error_("Renderer local exception encountered: {}", e.what());
  } catch (const std::exception &e) {
    ret_val = 2;
    Error_("Renderer non-local exception encountered: {}", e.what());
  }

  return ret_val;
}
