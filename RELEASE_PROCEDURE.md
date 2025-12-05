# Release Procedure

Follow these steps to create a new release:

1. **Run the Commit Release Script**

    Execute the commit release script to prepare the release commit and tag:

    ```bash
    ./scripts/commit_release.sh -t <new_version_tag>
    ```

2. **Push the New Tag and Branch**

    Push the new tag and branch to the remote repository:

    ```bash
    git push origin <new_version_tag>
    git push origin <new_release_branch>
    ```
