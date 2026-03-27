# Branch Protection Rules

## Main Branch (`main`)

### Protection Settings

| Setting | Value | Notes |
|---------|-------|-------|
| Require pull requests | ✅ Enabled | All changes must go through PR |
| Require approvals | ✅ 1 required | Human review required for agent changes |
| Dismiss stale reviews | ✅ Enabled | Auto-dismiss when new commits pushed |
| Require status checks | ✅ All CI jobs | build-firmware, host-tests, lint-* |
| Require linear history | ❌ Disabled | Keep merge commits for clarity |
| Allow force pushes | ❌ Disabled | No force pushes to main |

### Why These Rules?

- **Pull requests**: Ensures all changes are reviewed, even quick fixes
- **1 approval**: Single developer can still merge, but must consciously approve
- **Status checks**: CI must pass before merge - catches build errors
- **No force push**: Protects history integrity

## Feature Branches

| Setting | Value |
|---------|-------|
| Require pull requests | ✅ Enabled |
| Require approvals | ❌ Disabled |
| Require status checks | ❌ Disabled |
| Allow force pushes | ✅ Enabled |

Feature branches are for work-in-progress. They can be force-pushed freely and don't require approval.

## Agent (OpenCode) Branches

When the agent creates branches:
- Branch name format: `feature/*`, `fix/*`, `hotfix/*`
- Agent must create PR after CI passes
- Agent must wait for human approval before merge
- Agent should never merge its own PRs

## Manual Setup Required

Branch protection rules must be set via GitHub UI or terraform:

1. Go to **Settings → Branches → Add rule**
2. Enter `main` as branch name pattern
3. Enable the settings listed above
4. Under "Protect matching branches" → select required status checks:
   - `CI / build-firmware`
   - `CI / host-tests`
   - `CI / lint-cpp`
   - `CI / lint-markdown`

## Emergency Overrides

If you need to bypass protection (emergency hotfix):
1. Use GitHub UI → "Bypass branch protection" (requires admin rights)
2. This should rarely be needed
