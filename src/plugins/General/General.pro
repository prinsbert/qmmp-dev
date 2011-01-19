SUBDIRS += statusicon \
           notifier \
           scrobbler \
           fileops
unix:SUBDIRS += mpris \                
                hal \
                hotkey \
                covermanager \
                kdenotify
TEMPLATE = subdirs
