# AI Agents Usage & Governance

## 1. Purpose

Define how AI agents (LLM-driven assistants such as Copilot-like automated workers) may participate in the libmagicxx development lifecycle while preserving quality, safety, licensing, and contributor norms.

## 2. Scope

Agents may assist with: documentation, small feature scaffolding, test authoring, refactoring (non-ABI changing), build script improvements, and CI configuration suggestions. Human maintainers retain final responsibility for all merged changes.

## 3. Core Principles

1. Human Review Mandatory: No autonomous merging; every agent change must undergo a standard PR review.

2. Minimal Change Surface: Prefer localized, rationale-backed edits; avoid wide refactors without prior issue/RFC.

3. Reproducible Workflows: All build/test steps must use CMake workflow presets or `scripts/workflows.sh`.

4. ABI Caution: Avoid public header / symbol changes without explicit approval and changelog entry.

5. Transparent Attribution: Commits & PRs clearly mark agent role and prompt summary.

6. License Integrity: Do not introduce third‑party code with incompatible licenses (project is LGPL v3).

7. Security First: No secrets, credentials, or external network operations during builds/tests.

8. Documentation Coverage: New public APIs require full Doxygen comments & examples if meaningful.

## 4. Repository Context (from README & CONTRIBUTING)

Key directories:

+ `include/` (public API)

+ `sources/` (implementation)

+ `tests/` (GoogleTest)

+ `examples/`

+ `databases/` (magic db)

+ `cmake/` (config + targets)

+ `scripts/`

+ `documentation/`

+ `CMakePresets.json` (workflows)

Target language standard: C++23. Formatting enforced by `.clang-format` via preset `format-source-code`.

## 5. Supported Agent Roles

| Role | Description | Boundaries |
|------|-------------|-----------|
| docs | Improve README/examples/Doxygen commentary | Must not alter semantics or public API inadvertently |
| tests | Add/extend GoogleTest unit tests | Do not delete existing tests without justification |
| refactor | Internal code simplification (no ABI break) | Public headers stable; explain risk vs benefit |
| build-ci | Suggest preset adjustments or script hardening | Avoid removing existing supported toolchains |
| security | Flag potential unsafe patterns (unchecked I/O, error handling) | Advisory only, no auto-fixes without tests |
| enhancement | Small feature addition behind clear issue | Must create tests + docs; branch from `main` |
| quality | Formatting / static improvements | Use presets; do not reflow unrelated logic |

## 6. Branch & PR Conventions (from <a href="CONTRIBUTING.md">CONTRIBUTING.md</a>)

Branch naming per human guidelines (agents must follow):

+ Bug fixes: `bugfix/<brief>`

+ Documentation: `documentation/<brief>`

+ Enhancements: `enhancement/<brief>`

+ Code quality: `quality/<brief>`

For agent-specific work you may prefix role: e.g. `enhancement/agent-tests-flag-parser`.

PR Titles (include agent tag):

+ `[DOCUMENTATION][agent:docs] Brief Description, Fixes issue #1234.`

+ `[BUGFIX][agent:tests] Brief Description, Fixes issue #5678.`

Each PR must link an existing issue or include newly created one (agents may draft issue text, humans approve).

## 7. Build & Test Workflow Integration

Always rely on presets instead of ad‑hoc compiler flags.

Examples:

```bash
./scripts/workflows.sh -p linux-x86_64-clang-tests -c    # Clear cache + build + test
./scripts/workflows.sh -p format-source-code             # Format code
./scripts/workflows.sh -p linux-x86_64-clang-examples    # Build examples
cmake --workflow --preset linux-x86_64-clang-tests       # Direct CMake workflow alternative
```
Agents proposing changes MUST include a reproducible command list in PR description.

Helpful:

```bash
./scripts/workflows.sh -l                               # List available workflow presets
./scripts/workflows.sh -h                               # Show CLI usage for the wrapper
./scripts/workflows.sh -p linux-x86_64-gcc-tests -c     # GCC-based test preset
./scripts/workflows.sh -p linux-x86_64-clang-tests -c   # Clang-based test preset
```

Note: Prefer presets to toggling CMake options manually. If you must, ensure `BUILD_MAGICXX_TESTS=ON` when adding tests, and run the formatter via the `format-source-code` preset.

## 8. Development Container Usage

Environment is a Fedora-based dev container. Agents should not assume ability to run `git push/pull` inside container—the container doesn't have access to SSH keys or git credentials from the host machine (intentionally not shared for security). Provide host-side instructions when needed.

**Important:** Do not change the working directory when running terminal commands. Always use absolute paths or run commands from the repository root (`/libmagicxx`). Changing directories can cause subsequent commands to fail if they assume the original working directory.

Preset tips:

+ Run `python ./scripts/launch_container.py` from the host and attach with VS Code Dev Containers.

+ After attaching, initialize once via `./scripts/initialize.sh`.

## 9. Prompt Crafting Guidelines

Include: goal, constraints, touched paths, success criteria (tests/docs/format), prohibited areas (e.g., `include/` for refactors), and output format (unified diff or patch). Avoid vague commands like “optimize performance” without metric baseline.

Prompt Example (test addition):

```
Goal: Add regression test for invalid magic flag sequence (#123).
Constraints: New file in tests/, use existing GoogleTest patterns, no public API changes.
Success: Fails pre-fix, passes post-fix; command: ./scripts/workflows.sh -p linux-x86_64-clang-tests.
Output: Unified diff + run instructions.
```

## 10. Validation Checklist (Human Reviewer)

1. Builds succeed with declared preset(s).

2. All tests pass; new tests meaningful and isolated.

3. Formatting preset clean; no style drift.

4. No ABI-impactful changes (inspect public headers / symbols if modified).

5. Changelog updated if feature / refactor (keep versioning consistent with release procedure).

6. No license issues (no pasted third-party code without attribution & compatibility).

7. Documentation updates for new public API.

## 11. Security & Safety

Agents must avoid: leaking paths, embedding credentials, adding network downloads at build/test time. Use standard error handling, prefer exceptions or error codes consistent with existing code. Highlight risky file I/O additions for reviewer scrutiny.

## 12. Code Style & Conventions Recap

See <a href="STYLE_GUIDE.md">STYLE_GUIDE.md</a> for C++ conventions and <a href="cmake/STYLE_GUIDE.md">cmake/STYLE_GUIDE.md</a> for CMake conventions.

Key rules:

+ Classes, functions, namespaces, enums: `PascalCase`

+ Variables: `snake_case`; member variables: `m_` prefix (e.g., `m_handle`)

+ Template parameters: `PascalCase` with `T` suffix (types) or `V` suffix (non-types)

+ Type aliases: `PascalCase` ending with `T` (e.g., `FlagsMaskT`, `FileTypeT`)

+ Constants: `SCREAMING_SNAKE_CASE`

+ Files: `snake_case`; source files end with `.cpp`, headers end with `.hpp`

+ Doxygen identifiers: `snake_case` identifier + Title Case description (e.g., `@defgroup magic_core Core Magic API`)

+ C++23 features allowed; prefer clarity over cleverness.

+ Run formatting preset before committing.

+ Tests: follow existing GoogleTest naming and fixture patterns.

## 13. Artifact Attribution (Commit Footer Template)

```
Agent-Role: tests
Prompt-SHA256: <optional>
Scope: Add regression test for invalid flag parsing (#123)
Presets-Used: linux-x86_64-clang-tests
Validation: All tests pass; no public header changes.
```

## 14. Large / Multi-File Changes Protocol

For >400 LOC or cross-cutting refactors:

1. Draft Issue / Design (rationale, risk assessment).

2. Human approval before agent execution.

3. Split into incremental PRs (tests first, refactor next, docs last).

4. Track progress via checklist in PR description.

## 15. Example Agent Workflow

```
Issue #321 opened: Improve error reporting for load_database_file failures.
Agent (tests) adds failing test -> PR #322.
Human merges test PR.
Agent (refactor) adjusts error handling in sources/magic.cpp -> PR #323.
Agent (docs) updates README example -> PR #324.
Human reviews ABI + formatting; merges sequentially.
```

## 16. Prohibited Actions Without Prior Approval

+ Modifying licensing files (<a href="COPYING.LESSER">COPYING.LESSER</a>).

+ Changing container base or dependency install scripts drastically.

+ Adding new external dependencies.

+ Public API changes (headers in `include/`).

+ Deleting tests.

## 17. Adding a New Agent Role

1. Open issue describing function & boundaries.

2. Draft role entry (table + commit tag format).

3. Human approval.

4. Update this file.

5. Reference change in <a href="CHANGELOG.md">CHANGELOG.md</a>.

## 18. FAQ

**Can an agent perform release tagging?** No; use `scripts/commit_release.sh` under maintainer supervision.  

**Should agents modify `CMakePresets.json`?** Only to add clearly justified, documented presets; never remove existing ones without approval.  

**What if formatting differs?** Re-run `./scripts/workflows.sh -p format-source-code` and amend.

## 19. Quick Start (Agent Contribution)

1. Create/confirm issue describing task.

2. Craft precise prompt (section 9).

3. Execute changes on a properly named branch.

4. Run required presets locally; capture commands.

5. Open PR with attribution block & prompt text.

6. Human review & merge when checklist passes.
