# Libmagicxx Copilot Instructions

## Start Here

- Use [CONTRIBUTING.md](../CONTRIBUTING.md) for the development workflow, validation order, and PR expectations.
- Use [STYLE_GUIDE.md](../STYLE_GUIDE.md) for C++ naming, test naming, and Doxygen requirements.
- Use [cmake/STYLE_GUIDE.md](../cmake/STYLE_GUIDE.md) when editing CMake files.
- Use [README.md](../README.md) and [examples/magic_examples.cpp](../examples/magic_examples.cpp) to confirm intended public API usage.

## Clarifications & Interactive Behavior

- For ambiguous, incomplete, or scope-changing requests (for example: public API changes in `include/`, container changes, adding dependencies, deleting tests, or modifying license files), the agent MUST ask targeted clarifying questions using the `vscode/askQuestions` tool and wait for the user's explicit answers before making edits, running commands, or opening PRs.
- Keep questions concise and focused; prefer offering selectable options when appropriate. If the user does not respond, summarize the open questions and stop; do not proceed.

## Repository Map

- Public API lives in [include/magicxx](../include/magicxx). Treat changes there as API and ABI sensitive.
- Core library behavior is implemented primarily in [sources/magic.cpp](../sources/magic.cpp). Preserve the existing state model and PIMPL boundary.
- Tests live in [tests](../tests). Prefer updating the existing feature-specific `magic_*_test.cpp` file before creating a new one.
- Test sources are compiled into both shared and static test executables from the same source list. Test changes must compile cleanly for both targets.
- Build logic lives in [CMakeLists.txt](../CMakeLists.txt), [CMakePresets.json](../CMakePresets.json), and [cmake](../cmake).
- Target the project's documented C++23 standard.

## Build And Validation

- Prefer `./scripts/workflows.sh -p <preset>` or `cmake --workflow --preset <preset>` over ad hoc CMake commands.
- Run `./scripts/initialize.sh` before first build in a fresh container.
- Before proposing a code change, run this validation sequence in order and ensure all steps pass without errors:
  1. `./scripts/workflows.sh -p linux-x86_64-clang-tests -c`
  2. `./scripts/workflows.sh -p linux-x86_64-gcc-tests -c`
  3. `./scripts/workflows.sh -p format-source-code`
  4. `./scripts/workflows.sh -p clang-tidy-checks -c`
  5. `./scripts/workflows.sh -p documentation`

## Change Conventions

- Follow the documented naming rules: `PascalCase` for classes, functions, namespaces, enums, and type aliases; `snake_case` for variables; `m_` prefix for member variables.
- Keep source and header filenames in `snake_case`; tests should follow the existing `magic_*_test.cpp` pattern.
- Get explicit approval before public API changes under [include/magicxx](../include/magicxx) or large cross-cutting refactors.
- Do not add new external dependencies or third-party code without explicit approval.
- Do not delete tests without explicit approval.
- Do not modify licensing files such as [COPYING.LESSER](../COPYING.LESSER) without explicit approval.
- Public API additions or renames need Doxygen comments, including an accurate `@since` tag.
- All code changes must be documented with Doxygen comments. Follow the existing style and conventions in the codebase.
- Behavior changes should include or update regression tests in [tests](../tests).
- Changes should update [CHANGELOG.md](../CHANGELOG.md).
- If asked to create commits, use the component prefixes documented in [CONTRIBUTING.md](../CONTRIBUTING.md), such as `Magic:`, `Tests:`, `CMake:`, `Docs:`, or `Scripts:`.

## Environment Notes

- Work from the repository root. The project expects the build tree under [build](../build).
- Git push, pull, and fetch should be done from the host environment, not from inside the development container.
- Treat [Containerfile](../Containerfile) and [scripts/install_dependencies.sh](../scripts/install_dependencies.sh) changes as separate PR-sized work. Do not bundle them with unrelated fixes.
- Do not add network downloads to build or test workflows, and do not expose secrets or credentials in generated code, scripts, logs, or documentation.
- The development container is Fedora-based. Prefer repo scripts and documented presets over distro-specific ad hoc commands.
