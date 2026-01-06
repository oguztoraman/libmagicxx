# Release Procedure

Follow these steps to create a new release:

1. **Run the Commit Release Script**

    Execute the commit release script to prepare the release commit and tag:

    ```bash
    ./scripts/commit_release.sh -t <new_version_tag>
    ```

2. **Push the New Tag and Branch**

    Push the new tag and branch to the remote repository from your **host VS Code window** (not the container, which lacks git credentials).

    > 💡 **Tip:** Use the [Git Graph](https://marketplace.visualstudio.com/items?itemName=mhutchie.git-graph) extension to push tags and branches visually. Right-click on the tag or branch in the graph and select **"Push Tag..."** or **"Push Branch..."**.

    Alternatively, use the terminal on your host machine:

    ```bash
    git push origin <new_version_tag>
    git push origin <new_release_branch>
    ```
