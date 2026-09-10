# Working in Mengine

## Before making changes

- Read the [Mengine Codestyle](https://github.com/irov/Mengine/wiki/Codestyle) before changing code. It is the source of truth for coding conventions. Propose additions and clarifications there instead of duplicating coding rules in this file.
- Inspect the existing interfaces, helpers, and component responsibilities relevant to the task before adding an implementation.
- Treat existing code as context, not as permission to repeat violations of the codestyle.
- Inspect the working tree and distinguish your changes from existing work. Preserve unrelated changes, including staged changes, and keep edits within the task's scope.

## While making changes

- Treat every fix and feature as part of an engine: keep solutions concise, cohesive, and idiomatic to Mengine's architecture. Add abstractions, state, and special cases only when the required behavior justifies them.
- Reuse the appropriate existing abstractions and keep behavior in the component responsible for it.
- Keep related APIs, operations, and lifecycles symmetric where their contracts match. Introduce differences only when behavior or ownership requires them.
- When changing a shared interface, inspect and update all affected implementations and callers, including platform-specific and optional components.
- Preserve ownership and lifecycle contracts. Handle cleanup after partial initialization and release resources, registrations, and callbacks at the appropriate lifecycle stage.
- Preserve the encoding and line endings of edited files. Avoid unrelated formatting changes.
- When a review identifies a mistake, check all analogous places in your own changes and fix the same mistake consistently.
- Document general project conventions rather than turning a solution to one task into a project-wide requirement.

## Before finishing

- Review your own diff against the Codestyle, including examples in any documentation you changed.
- Review the design for unnecessary complexity. If a solution is bulky, reconsider its responsibilities and use existing engine mechanisms to express the same behavior more directly, with fewer concepts, state variables, and branches. Simplify while preserving clarity, contracts, and required performance.
- Run checks appropriate to the change and inspect their results.
- Use the existing build scripts and configurations appropriate to the affected components. Report the target platform, build configuration, and relevant options used for verification.
- Fix compiler warnings introduced by your changes rather than suppressing them. Report pre-existing warnings separately and keep unrelated fixes outside the task's scope.
- Check both unstaged and staged diffs for whitespace errors without changing the files' line endings:

  ```bash
  git -c core.whitespace=cr-at-eol diff --check
  git -c core.whitespace=cr-at-eol diff --cached --check
  ```

- Distinguish whitespace issues in your own changes from those in existing or unrelated work. Review new, untracked files separately because these diff commands do not cover them.
- Verify that unrelated work remains intact.
- Report what changed and what was actually verified. Distinguish compilation of individual files, a complete build, automated tests, and runtime validation; state any checks that could not be completed.
