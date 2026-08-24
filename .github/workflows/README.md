# SVN-to-Git sync

A GitHub Actions workflow to sync an SVN repository with its Git mirror every once in a while.

Each run:

1. Clones this repository as it currently stands.
2. Finds the last SVN revision already mirrored, by reading the `git-svn-id: ...@<rev>` trailer on the tip of `master`.
3. Asks SVN for the log of every revision since then.
4. Replays each new revision as a Git commit: SVN's `trunk` becomes the
   `master` branch, each `branches/<name>` becomes a Git branch of the same
   name, and each `tags/<name>` becomes a Git tag.
5. Force-pushes the result back to this repository.

The initial run looks something like this:
```
mkdir -p mirror
git svn clone $STDLAYOUT "$SVN_URL" mirror
```

Although I do not recommend that you run this command yourself. `git-svn` has some malfunction which causes it to not memoize revision history and re-run the entire revision history for each branching, so expect this command to take 6-12 hours. For incremental updates, this becomes prohibitively expensive.

`.github/workflows/svn-mirror.yml` therefore does not use `git-svn`, but manually tries to reconcile new SVN revisions with Git commits. This comes with a secondary benefit. `git-svn` keeps its own binary bookkeeping (a revision→commit map) inside `.git/svn`, which has to be persisted and restored between CI runs. Any drift between that bookkeeping and what's actually pushed to GitHub was a repeated source of hard-to-diagnose failures. In contrast, this Github workflow derives state directly from the pushed Git history (specifically the `git-svn-id` trailer), so there's no separate cache that must be maintained, can go stale or corrupted.

## Known limitations

- **File properties** such as `svn:executable` and `svn:eol-style` are not carried over.
- **Tags/branches** are cut from the current tip of their source branch at sync time, not from the exact SVN `copyfrom` revision. They are expected to be similar, but it isn't guaranteed.
- **The resume point** is read only from `master` (SVN: `trunk`). If a run of revisions touches only branches and not `trunk`, those revisions get reprocessed (harmlessly, but wastefully) on the next run until `trunk` is touched again.
