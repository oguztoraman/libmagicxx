---
name: create-implementation-plan
description: 'Create a new implementation plan file for new features, refactoring existing code or upgrading packages, design, architecture or infrastructure.'
---

# Create Implementation Plan

## Primary Directive

Your goal is to create a new implementation plan file for `${input:PlanPurpose}`. You MUST follow the **Mandatory Workflow** below in strict order. Do not skip any step, do not proceed to the next step until the current step is complete, and do not write the plan file until all user decisions have been confirmed.

---

## Mandatory Workflow

Execute the following steps **in order**. Each step is a gate — do not advance until it is done.

### Step 1 — Explore the Codebase

Before anything else, use `semantic_search`, `grep_search`, and `read_file` to understand:
- The files and components directly related to `${input:PlanPurpose}`
- The current state of the code (what exists, what is missing, what is broken)
- Existing naming conventions, patterns, and constraints in the repository
- Any prior attempts or related work already in the codebase

Collect enough context to formulate precise web search queries in Step 3.

### Step 2 — Ask Clarifying Questions (BEFORE Searching the Web)

Use the `vscode_askQuestions` tool to ask the user targeted clarifying questions that will focus the web search. Questions MUST cover:

1. **Scope**: What exactly should the plan cover? What is out of scope?
2. **Constraints**: Are there language version, dependency, ABI, or API constraints to respect?
3. **Goals**: What does "done" look like? Are there performance, safety, or compatibility requirements?
4. **Prior decisions**: Has the user already decided on an approach? Should the plan validate it or explore alternatives?

Use `options` arrays in each question where the answer space is bounded. Use `multiSelect: true` when multiple choices are valid simultaneously. Do NOT proceed to Step 3 until the user has answered all questions.

### Step 3 — Search the Web for Best Practices

Using the answers from Step 2 and the codebase context from Step 1, construct specific search queries. Use the `fetch_webpage` tool to retrieve content from:
- Official documentation (cppreference, isocpp.org, language-specific references)
- Authoritative community sources (GitHub discussions, Stack Overflow accepted answers, CppCon/conference talks)
- C++ Core Guidelines or equivalent authoritative style/pattern guides relevant to the topic

For each major design decision identified in Step 2, find **3 to 5 concrete options** with real-world backing. Document:
- Option name and brief description
- Source URL
- Key tradeoff (pros/cons)
- Recommendation level (widely adopted / niche / deprecated)

### Step 4 — Present Options and Ask User to Choose

Use the `vscode_askQuestions` tool to present the options found in Step 3 to the user. For each decision point:
- Show a question with the option names as `options` array entries
- Include source/rationale in the `message` field (Markdown is supported)
- Use `multiSelect: false` unless the decision allows combining multiple approaches
- Set `recommended: true` on the most community-endorsed option

**Do NOT write the plan file until the user has selected an option for every decision point.** If the user skips a question, flag it as unresolved and ask again before proceeding.

### Step 5 — Generate the Plan File

Only after Steps 1–4 are complete, generate the implementation plan file using the **Mandatory Template Structure** below. Apply the user's selections from Step 4 as the chosen approach. Document all rejected alternatives in Section 3 (Alternatives) with the reason they were not chosen.

**Output file location**: `plan/` directory at the repository root.
**Naming convention**: `[purpose]-[component]-[version].md`
**Purpose prefixes**: `bugfix|upgrade|refactor|feature|data|infrastructure|process|architecture|design`
**Examples**: `feature-magic-error-type-1.md`, `refactor-pimpl-cleanup-2.md`

---

## Execution Context

This prompt is designed for AI-to-AI communication and automated processing. All instructions must be interpreted literally and executed systematically.

## Core Requirements

- Generate implementation plans that are fully executable by AI agents or humans
- Use deterministic language with zero ambiguity
- Structure all content for automated parsing and execution
- Ensure complete self-containment with no external dependencies for understanding

## Phase Architecture

- Each phase must have measurable completion criteria
- Tasks within phases must be executable in parallel unless dependencies are specified
- All task descriptions must include specific file paths, function names, and exact implementation details
- No task should require human interpretation or decision-making

## AI-Optimized Implementation Standards

- Use explicit, unambiguous language with zero interpretation required
- Structure all content as machine-parseable formats (tables, lists, structured data)
- Include specific file paths, line numbers, and exact code references where applicable
- Define all variables, constants, and configuration values explicitly
- Provide complete context within each task description
- Use standardized prefixes for all identifiers (REQ-, TASK-, etc.)
- Include validation criteria that can be automatically verified

---

## Mandatory Template Structure

All implementation plans must strictly adhere to the following template. Each section is required and must be populated with specific, actionable content derived from Steps 1–4 of the Mandatory Workflow. AI agents must validate template compliance before saving the file.

### Template Validation Rules

- All front matter fields must be present and properly formatted
- All section headers must match exactly (case-sensitive)
- All identifier prefixes must follow the specified format
- Tables must include all required columns
- No placeholder text may remain in the final output
- Section 3 (Alternatives) must list every option presented in Step 4 that the user did not select, with the reason

### Status Values

The `status` field must reflect the current state. Use the following values and badge colors:

| Status | Badge Color |
|--------|-------------|
| `Completed` | `brightgreen` |
| `In progress` | `yellow` |
| `Planned` | `blue` |
| `Deprecated` | `red` |
| `On Hold` | `orange` |

```md
---
goal: [Concise Title Describing the Package Implementation Plan's Goal]
version: [Optional: e.g., 1.0, Date]
date_created: [YYYY-MM-DD]
last_updated: [Optional: YYYY-MM-DD]
owner: [Optional: Team/Individual responsible for this spec]
status: 'Completed'|'In progress'|'Planned'|'Deprecated'|'On Hold'
tags: [Optional: List of relevant tags or categories, e.g., `feature`, `upgrade`, `chore`, `architecture`, `migration`, `bug` etc]
---

# Introduction

![Status: <status>](https://img.shields.io/badge/status-<status>-<status_color>)

[A short concise introduction to the plan and the goal it is intended to achieve.]

## 1. Requirements & Constraints

[Explicitly list all requirements & constraints that affect the plan and constrain how it is implemented. Use bullet points or tables for clarity.]

- **REQ-001**: Requirement 1
- **SEC-001**: Security Requirement 1
- **[3 LETTERS]-001**: Other Requirement 1
- **CON-001**: Constraint 1
- **GUD-001**: Guideline 1
- **PAT-001**: Pattern to follow 1

## 2. Implementation Steps

### Implementation Phase 1

- GOAL-001: [Describe the goal of this phase, e.g., "Implement feature X", "Refactor module Y", etc.]

| Task | Description | Completed | Date |
|------|-------------|-----------|------|
| TASK-001 | Description of task 1 | ✅ | 2025-04-25 |
| TASK-002 | Description of task 2 | |  |
| TASK-003 | Description of task 3 | |  |

### Implementation Phase 2

- GOAL-002: [Describe the goal of this phase, e.g., "Implement feature X", "Refactor module Y", etc.]

| Task | Description | Completed | Date |
|------|-------------|-----------|------|
| TASK-004 | Description of task 4 | |  |
| TASK-005 | Description of task 5 | |  |
| TASK-006 | Description of task 6 | |  |

## 3. Alternatives

[A bullet point list of any alternative approaches that were considered and why they were not chosen. This helps to provide context and rationale for the chosen approach.]

- **ALT-001**: Alternative approach 1
- **ALT-002**: Alternative approach 2

## 4. Dependencies

[List any dependencies that need to be addressed, such as libraries, frameworks, or other components that the plan relies on.]

- **DEP-001**: Dependency 1
- **DEP-002**: Dependency 2

## 5. Files

[List the files that will be affected by the feature or refactoring task.]

- **FILE-001**: Description of file 1
- **FILE-002**: Description of file 2

## 6. Testing

[List the tests that need to be implemented to verify the feature or refactoring task.]

- **TEST-001**: Description of test 1
- **TEST-002**: Description of test 2

## 7. Risks & Assumptions

[List any risks or assumptions related to the implementation of the plan.]

- **RISK-001**: Risk 1
- **ASSUMPTION-001**: Assumption 1

## 8. Related Specifications / Further Reading

[Link to related spec 1]
[Link to relevant external documentation]
```
