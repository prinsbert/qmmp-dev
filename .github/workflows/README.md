# SVN-to-Git sync

A GitHub Actions workflow (`.github/workflows/svn-mirror.yml`) runs on a schedule. 

Each run:

1. Clones this repository as it currently stands.
2. Finds the last SVN revision already mirrored, by reading the `git-svn-id: ...@<rev>` trailer on the tip of `master`.
3. Asks SVN for the log of every revision since then.
4. Replays each new revision as a Git commit: SVN's `trunk` becomes the
   `master` branch, each `branches/<name>` becomes a Git branch of the same
   name, and each `tags/<name>` becomes a Git tag.
5. Force-pushes the result back to this repository.

This sync is a small, self-contained Python script rather than `git svn`.
`git svn` keeps its own binary bookkeeping (a revision→commit map) that has
to be persisted and restored between CI runs, and any drift between that
bookkeeping and what's actually pushed to GitHub was a repeated source of
hard-to-diagnose failures. The script here instead derives its state
directly from the pushed Git history each run (the `git-svn-id` trailer),
so there's no separate cache that can go stale or get corrupted.

## Known limitations

- **File properties** such as `svn:executable` and `svn:eol-style` are not
  carried over.
- **Tags/branches** are cut from the current tip of their source branch at
  sync time, not from the exact SVN `copyfrom` revision — in the vast
  majority of cases these are the same thing, but it isn't guaranteed.
- **The resume point** is read only from `master`. If a run of revisions
  touches only branches and not `trunk`, those revisions get reprocessed
  (harmlessly, but wastefully) on the next run until `trunk` is touched
  again.
- **No automatic retries** around SVN/network calls — a transient failure
  fails the whole run, which simply tries again at the next scheduled
  interval.
