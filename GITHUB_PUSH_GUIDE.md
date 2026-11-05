# GitHub Push Guide — Kontroller

> Quick reference for common git operations. See [README.md](README.md) for contribution workflow.

---

## Case 0: `.gitignore` — Commit First
```bash
git add .gitignore
git commit -m "chore: update .gitignore"
```
Prevents accidental commit of `.pio/`, build artifacts, IDE configs.

---

## Case 1: First-Time Setup (Fresh Repo)
```bash
git init
git config user.name "Your Name"
git config user.email "you@example.com"
git remote add origin https://github.com/USERNAME/Kontroller.git
git add .gitignore platformio.ini src/Kontroller.ino laptop_bridge.py .vscode/extensions.json README.md LICENSE
git commit -m "Initial commit: ESP32 3-motor controller + laptop bridge"
git branch -M main
git push -u origin main
```

---

## Case 2: Daily Push
```bash
git pull origin main --rebase
git add -A
git commit -m "feat: add battery monitoring via ADC"
git push origin main
```

---

## Case 3: Feature Branch Workflow
```bash
git checkout -b feat/omni-wheel-support
# work, test, commit
git push -u origin feat/omni-wheel-support
# Open PR on GitHub → review → merge
git checkout main
git pull origin main
git branch -d feat/omni-wheel-support
git push origin --delete feat/omni-wheel-support
```

---

## Case 4: Tagging Releases
```bash
git tag -a v1.0.0 -m "Release v1.0.0 - Initial stable firmware"
git push origin v1.0.0
# or all tags:
git push origin --tags
```

---

## Case 5: Arduino IDE + PlatformIO Dual-Use Checklist
| File | PlatformIO | Arduino IDE | Notes |
|------|------------|-------------|-------|
| `src/Kontroller.ino` | ✅ | ✅ | Folder must match `.ino` name |
| `platformio.ini` | ✅ | ❌ | Keep |
| `.vscode/extensions.json` | ✅ | ❌ | Only this file tracked |
| `laptop_bridge.py` | ✅ | ✅ | Standalone |

---

## Case 6: Troubleshooting
```bash
# Remote already exists
git remote remove origin
git remote add origin <new-url>

# Divergent branches
git pull origin main --rebase
# resolve conflicts
git rebase --continue
git push

# Fix last commit (local only)
git add .
git commit --amend --no-edit
git push --force-with-lease origin main

# Permission denied (SSH)
ssh -T git@github.com
# Add key to GitHub Settings → SSH Keys
```

---

## Quick Reference
```bash
# Daily
git pull --rebase && git add -A && git commit -m "msg" && git push

# New feature
git checkout -b feat/x && git push -u origin feat/x

# Release
git tag -a v1.2.0 -m "msg" && git push origin v1.2.0

# Fix last commit
git add . && git commit --amend --no-edit && git push --force-with-lease

# What will push
git log origin/main..HEAD --oneline
```