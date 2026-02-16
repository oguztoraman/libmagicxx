# Pull Request Template

## Description

Detailed description...

Fixes issue #????.

...

## Checklist

- [ ] Branch created from `main` (or release branch for bugfix)
- [ ] Code follows <a href="STYLE_GUIDE.md">STYLE_GUIDE.md</a>
- [ ] Clang tests pass: `./scripts/workflows.sh -p linux-x86_64-clang-tests -c`
- [ ] GCC tests pass: `./scripts/workflows.sh -p linux-x86_64-gcc-tests -c`
- [ ] Code formatted: `./scripts/workflows.sh -p format-source-code`
- [ ] Clang-tidy checks pass: `./scripts/workflows.sh -p clang-tidy-checks -c`
- [ ] Documentation generates cleanly: `./scripts/workflows.sh -p documentation`
- [ ] New tests added for new functionality
- [ ] Doxygen comments added for new public APIs
- [ ] CHANGELOG.md updated
- [ ] Linked to an existing issue (or created one)

### Title Format Guidelines

+ For bug fixes: `[BUGFIX] Brief Description, Fixes issue #????.`
+ For documentation changes: `[DOCUMENTATION] Brief Description, Fixes issue #????.`
+ For enhancements: `[ENHANCEMENT] Brief Description, Fixes issue #????.`
+ For code quality improvements: `[QUALITY] Brief Description, Fixes issue #????.`
