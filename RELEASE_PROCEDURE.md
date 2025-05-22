# Release Procedure

Follow these steps to create a new release:

1. **Update the Security Policy**

    Edit `SECURITY.md` to update the supported versions table and any other relevant information.

2. **Run the Publish Release Script**

    Execute the release script to prepare and publish the release commit and tag:

    ```bash
    ./scripts/publish_release.sh -t <new_version_tag>
    ```
