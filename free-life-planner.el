;; (add-hook 'shell-mode-hook (lambda () (local-unset-key "\C-c\C-e")))

(global-set-key "\C-c\C-k\C-vFLP" 'flp-quick-start)

;; ;; start: like C-cre(r)
;; (global-set-key "\C-cffe" 'flp-)

;; ;; end: like C-cre(r)

(global-set-key "\C-cffE" 'flp-edit-flp-elisp)

(global-set-key "\C-cffcm" 'flp-open-model)
(global-set-key "\C-cffcV" 'flp-open-view)
(global-set-key "\C-cffcc" 'flp-open-controller)
(global-set-key "\C-cffct" 'flp-open-template)

(global-set-key "\C-cffsm" 'flp-search-models)
(global-set-key "\C-cffsv" 'flp-search-views)
(global-set-key "\C-cffsc" 'flp-search-controllers)
(global-set-key "\C-cffst" 'flp-search-templates)

(global-set-key "\C-cffcp" 'flp-auto-commit)
(global-set-key "\C-cffcl" 'flp-auto-commit-large)
(global-set-key "\C-cffcv" 'flp-auto-commit-private)
(global-set-key "\C-cffpp" 'flp-pull-push)
(global-set-key "\C-cffpl" 'flp-pull-push-large)
(global-set-key "\C-cffbpv" 'flp-pull-push-private)

(global-set-key "\C-cffit" 'flp-insert-prolog-timestamp)
(global-set-key "\C-cffiT" 'flp-insert-prolog-timestamp-for-time)
(global-set-key "\C-cffid" 'flp-insert-prolog-datestamp)
(global-set-key "\C-cffiD" 'flp-insert-prolog-datestamp-for-date)
(global-set-key "\C-cfff" 'flp-load-tasks)
(global-set-key "\C-cffX" 'flp)
(global-set-key "\C-cffwS" 'flp-set-windows)
(global-set-key "\C-cffws" 'flp-set-windows-with-additionals)
(global-set-key "\C-cffwo" 'flp-open-todos)
(global-set-key "\C-cffwc" 'flp-open-cycs)
(global-set-key "\C-cffwd" 'flp-cyc-open-defaults)
(global-set-key "\C-cffwa" 'flp-open-akahige)
(global-set-key "\C-cffwv" 'flp-open-verber)
(global-set-key "\C-cffwV" 'flp-open-verber-edit-domains)

(global-set-key "\C-cffk" 'flp-kill-interactive)
(global-set-key "\C-cffK" 'flp-kill-wsm-process)
(global-set-key "\C-cffr" 'flp-restart)
(global-set-key "\C-cffR" 'flp-restart-fcms)
;; (global-set-key "\C-cffR" 'flp-restart-old)
(global-set-key "\C-cffn" 'flp-new-task)

(global-set-key "\C-ceS" 'flp-true)
(global-set-key "\C-cesr" 'flp-true-restart)
(global-set-key "\C-cffort" 'flp-true-reload-current-source-file)


;; (global-set-key [?\C-\t] 'flp-complete-or-tab)
;; (global-set-key (kbd "<C-tab>") 'flp-complete-or-tab)
;; (global-set-key (kbd "<tab>") 'flp-complete-or-tab)
(global-set-key "\C-cfftt" 'flp-complete-or-tab)
(global-set-key "\C-cffta" 'flp-constant-apropos-completing-read)
(global-set-key "\C-cfftp" 'perllib-perl-prolog-or-formalog-prolog-mode)

(global-set-key "\C-cffogs" 'flp-grep-source-files)
(global-set-key "\C-cffogS" 'flp-grep-source-file-names)
(global-set-key "\C-cffogP" 'flp-grep-source-files-reload)
(global-set-key "\C-cffos" 'flp-search-source-file-names)
(global-set-key "\C-cffop" 'flp-open-source-file)
(global-set-key "\C-cffoP" 'flp-open-source-file-reload)
(global-set-key "\C-cfforr" 'flp-reload-current-source-file)
(global-set-key "\C-cffm" 'flp-reload-all-modified-source-files)
(global-set-key "\C-cffol" 'flp-list-source-files)
(global-set-key "\C-cffoS" 'flp-current-buffer-flp-source-file-p)

(global-set-key "\C-cffof" 'flp-open-frdcsa-sys-flp-file)
(global-set-key "\C-cffoF" 'flp-open-frdcsa-sys-flp-autoload-file)

(global-set-key "\C-cffOin" 'flp-open-incoming)
(global-set-key "\C-cffOrd" 'flp-open-productivity)
(global-set-key "\C-cffOgo" 'flp-open-gourmet)
(global-set-key "\C-cffOlm" 'flp-open-loan-manager)
(global-set-key "\C-cffOsl" 'flp-open-shopping-list)
(global-set-key "\C-cffOama" 'flp-open-audience-master-agent)
(global-set-key "\C-cffOasm" 'flp-open-audience-agent-snail-mail)
(global-set-key "\C-cffOaph" 'flp-open-audience-agent-phone)
(global-set-key "\C-cffOaem" 'flp-open-audience-agent-email)
(global-set-key "\C-cffOaip" 'flp-open-audience-agent-in-person)
(global-set-key "\C-cffOfw" 'flp-open-free-wopr)

(global-set-key (kbd "C-c f f TAB") 'flp-complete-from-predicates-in-current-buffer)
(global-set-key "\C-cffL" 'flp-log)
(global-set-key "\C-cfflco" 'flp-complete-log)
(global-set-key "\C-cfflgo" 'flp-log-gourmet)
(global-set-key "\C-cfflpo" 'flp-log-productivity)
(global-set-key "\C-cffllm" 'flp-log-loan-manager)
(global-set-key "\C-cfflsl" 'flp-log-shopping-list)
(global-set-key "\C-cfflsl" 'flp-log-shopping-list)
(global-set-key "\C-cfflama" 'flp-log-audience-master-agent)
(global-set-key "\C-cfflasm" 'flp-log-audience-agent-snail-mail)
(global-set-key "\C-cfflaph" 'flp-log-audience-agent-phone)
(global-set-key "\C-cfflaem" 'flp-log-audience-agent-email)
(global-set-key "\C-cfflaip" 'flp-log-audience-agent-in-person)
(global-set-key "\C-cffle" 'flp-list-errors)
(global-set-key "\C-cfflE" 'flp-list-errors-with-current-file)
(global-set-key "\C-cfflm" 'flp-list-multifile-and-dynamic)
(global-set-key "\C-cfflM" 'flp-edit-multifile-and-dynamic-file)
(global-set-key "\C-cfflu" 'flp-list-unimplemented-yet-referenced-predicates)
(global-set-key "\C-cfflw" 'flp-edit-speech-dialog-interface-questions-list)

;; (defvar fcms-port-values ai.frdcsa.org, 3000, panoply2.frdcsa.org, 3001)
(defvar flp-complete-from-predicates nil)

(defun flp-edit-flp-elisp ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/free-life-planner/free-life-planner.el"))

(defun flp-commit ()
 ""
 (interactive)
 (run-in-shell "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/commit.sh" "*commit.sh*"))

(defun flp-auto-commit (&optional arg)
 ""
 (interactive "P")
 (run-in-shell
  (concat
   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/auto-commit.sh"
   (if arg " extra" ""))
  "*auto-commit*"))

(defun flp-auto-commit-large (&optional arg)
 ""
 (interactive "P")
 (run-in-shell
  (concat
   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/auto-commit-large.sh"
   (if arg " extra" ""))
  "*auto-commit-large*"))

(defun flp-auto-commit-private (&optional arg)
 ""
 (interactive "P")
 (run-in-shell
  (concat
   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/auto-commit-private.sh"
   (if arg " extra" ""))
  "*auto-commit-large*"))

(defun flp-pull-push (&optional arg)
 ""
 (interactive "P")
 (kmax-ask-for-password-and-push-on-kill-ring "posi.frdcsa.org")
 (run-in-shell
  (concat
   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/pull-push.sh"
   (if arg " extra" ""))
  "*pull-push*"))


(defun flp-pull-push-large (&optional arg)
 ""
 (interactive "P")
 (kmax-ask-for-password-and-push-on-kill-ring "posi.frdcsa.org")
 (run-in-shell
  (concat
   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/pull-push-large.sh"
   (if arg " extra" ""))
  "*pull-push-large*"))

(defun flp-pull-push-private (&optional arg)
 ""
 (interactive "P")
 (kmax-ask-for-password-and-push-on-kill-ring "posi.frdcsa.org")
 (run-in-shell
  (concat
   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/pull-push-private.sh"
   (if arg " extra" ""))
  "*pull-push-large*"))

(defun flp-insert-prolog-timestamp ()
 ""
 (interactive)
 (insert (flp-prolog-timestamp)))

(defun flp-insert-prolog-timestamp-for-time ()
 ""
 (interactive)
 (kmax-fixme
  "Need to have this ask for a time relative to our current date, such as, in three days, or in 20 minutes, and return the appropriate timestamp"
  t)
 ;; (insert (flp-prolog-timestamp))
 )

(defun flp-insert-prolog-datestamp ()
 ""
 (interactive)
 (insert (flp-prolog-datestamp)))

(defun flp-insert-prolog-datestamp-for-date ()
 ""
 (interactive)
 (kmax-fixme
  "Need to have this ask for a date relative to our current date, such as, in three days, or in 20 minutes, and return the appropriate datestamp"
  t)
 ;; (insert (flp-prolog-datestamp))
 )

(defun flp-prolog-timestamp ()
 (chomp (shell-command-to-string "date \"+[%Y-%m-%d,%H:%M:%S]\"")))

(defun flp-prolog-datestamp ()
 (chomp (shell-command-to-string "date \"+[%Y-%m-%d]\"")))

(defun flp-load-tasks ()
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/fluents_data.pl")
 (end-of-buffer))

(defvar flp-source-files nil)
(defvar flp-source-files-chase-alist nil)

(defun flp-reload-source-files-chase-alist ()
 ""
 (interactive)
 (setq flp-source-files nil)
 (flp-load-source-files)
 (see flp-source-files-chase-alist))

(defun flp-load-source-files ()
 ""
 (if (not (non-nil flp-source-files))
  (progn
   (setq flp-source-files
    (cdr
     (nth 1
      (nth 0
       (formalog-query (list 'var-X) (list "listFiles" 'var-X))))))
   (setq flp-source-files-chase-alist (kmax-file-list-chase-alist flp-source-files)))))

(defun flp-grep-source-file-names ()
 (interactive)
 (let ((regex (read-from-minibuffer "Search?: ")))
  (freekbs2-push-onto-stack (kmax-grep-list-regexp (flp-get-actual-source-files) regex) nil)))

(defun flp-grep-source-files-reload ()
 (interactive)
 (setq flp-source-files nil)
 (flp-grep-source-files))

(defun flp-grep-source-files (&optional arg search-arg)
 ""
 (interactive "P")
 (flp-load-source-files)
 (let ((search (if arg (thing-at-point 'symbol)
		(or search-arg (read-from-minibuffer "Search?: "))))
       (file-containing-files-to-search "/tmp/free-life-planner-search-files.txt"))
  (kmax-write-string-to-file (join "\n" (flp-get-actual-source-files)) file-containing-files-to-search)
  (run-in-shell
   (concat "tr '\\n' '\\0' <" file-containing-files-to-search " | LANG=C xargs -r0 grep -iF " (shell-quote-argument search))
   "*FLP-Search*")))

(defun flp-grep-source-files-orig ()
 ""
 (interactive)
 (flp-load-source-files)
 (let ((search (read-from-minibuffer "Search?: ")))
  (run-in-shell
   (concat "grep -iE \"" (shell-quote-argument search) "\" "
    (join " "
     (flp-get-actual-source-files)))
   "*FLP-Search*")))

(defun flp-get-actual-source-files ()
 ""
 (mapcar 'shell-quote-argument
  (kmax-grep-list flp-source-files
   (lambda (value)
    (and (stringp value) (file-exists-p value))))))

(defun flp-list-source-files ()
 (interactive)
 (setq flp-source-files nil)
 (flp-load-source-files)
 (see flp-source-files))

(defun flp-open-source-file-reload ()
 (interactive)
 (setq flp-source-files nil)
 (flp-open-source-file)
 (flp-get-actual-source-files))

(defun flp-open-source-file ()
 (interactive)
 (flp-load-source-files)
 (let ((file (ido-completing-read "Source File: " (flp-get-actual-source-files))))
  (ffap file)
  (end-of-buffer)
  (flp-complete-from-predicates-in-current-buffer)))

(defun flp-open-free-wopr ()
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/free_wopr.pl")
 (end-of-buffer))

(defun flp-new-task ()
 (interactive)
 (flp-load-tasks)
 (insert "task(task,'',critical).")
 (backward-char 14))

(defun flp-set-windows ()
 (interactive)
 (flp-fix-windows)
 (flp-select-windows))

(defvar flp-latest-file "/home/andrewdo/.config/frdcsa/todo/latest.do")
(defvar flp-todo-file "/home/andrewdo/to.do")

(defun flp-set-windows-with-additionals (&optional arg)
 (interactive "P")
 (if (not (flp-buffers-open-p))
  (error "ERROR: FLP not running.")
  (progn
   (flp-set-windows)
   (other-window 1)
   (ffap flp-latest-file)
   (other-window 1)
   (split-window-below)
   (switch-to-buffer (cmhsm-current-connection-fn))
   (other-window 2)
   (split-window-below)
   (switch-to-buffer "*WSM*")
   (other-window 2)
   (split-window-below)
   (switch-to-buffer "*Inform7*")
   (pop-to-buffer "*Formalog-REPL*")
   (if arg
    (kmax-map-end-of-buffer-and-recenter-top-bottom-over-all-visible-buffers (list flp-latest-file))))))

(defun flp-fix-windows ()
 ""
 (interactive)
 (delete-other-windows)
 (split-window-vertically)
 (split-window-horizontally)
 (other-window 2)
 (split-window-horizontally)
 (other-window -2))

(defun flp-select-windows ()
 ""
 (interactive)
 (switch-to-buffer "*WSM*")
 (other-window 1)
 (switch-to-buffer "*ushell*")
 (other-window 1)
 (switch-to-buffer "*FCMS*")
 (other-window 1)
 (switch-to-buffer "*Formalog-REPL*"))

(defvar flp-process-patterns
 (list
  "/var/lib/myfrdcsa/codebases/internal/unilang/unilang"
  "/var/lib/myfrdcsa/codebases/internal/unilang/unilang-client"
  "-Djava.library.path=/var/lib/myfrdcsa/sandbox/[^/]+/[^/]+/api/java/lib org.perl.inline.java.InlineJavaServer"
  "/var/lib/myfrdcsa/codebases/internal/freekbs2/kbs2-server"
  "/var/lib/myfrdcsa/codebases/internal/freekbs2/data/theorem-provers/vampire/Vampire1/Bin/server.pl"
  "perl ./script/shinycms_server.pl"
  "/bin/bash ./begin-fcms"
  "/var/lib/myfrdcsa/codebases/minor/inform7-frdcsa/9000"
  "/var/lib/myfrdcsa/codebases/minor/inform7-frdcsa/hal"
  "/var/lib/myfrdcsa/sandbox/glulxe-0.5.2/glulxe-0.5.2/glulxe"))

(defun flp-buffers-open-p ()
 (interactive)
 (and
  (kmax-buffer-exists-p "*WSM*")
  (kmax-buffer-exists-p "*FCMS*")
  (kmax-buffer-exists-p "*ushell*")
  (kmax-buffer-exists-p "*Formalog-REPL*")
  ;; (shell-command "ps auxwww | grep \"perl ./script/shinycms_server.pl -r -h \" | grep \" -p 3000 -d\" | awk '{print \$2}' | grep -q .")
  ))

(defvar flp-running nil)

(defun flp-running-p ()
 (interactive)
 (setq flp-running-tmp t)
 (let* ((matches nil)
	(processes (split-string (shell-command-to-string "ps auxwww") "\n"))
	(failed nil))
  (mapcar 
   (lambda (process)
    (if (not (kmax-util-non-empty-list-p (kmax-grep-v-list-regexp (kmax-grep-list-regexp processes process) "grep")))
     (progn
      (see process 0.0)
      (setq flp-running-tmp nil)
      (push process failed))))
   flp-process-patterns)
  (setq flp-running flp-running-tmp)
  (if (kmax-util-non-empty-list-p failed)
   (see failed 0.1))
  flp-running))

(defun flp-list-processes ()
 (interactive)
 (setq flp-running-tmp t)
 (let* ((matching-processes nil)
	(processes (split-string (shell-command-to-string "ps auxwww") "\n")))
  (mapcar 
   (lambda (process)
    (mapcar
     (lambda (process-listing-line)
      (string-match "^\\([a-zA-Z0-9]+\\)[ ]+\\([0-9]+\\)[ ]+" process-listing-line)
      (push (match-string 2 process-listing-line)  matching-processes))
     (kmax-grep-list-regexp processes process)))
   flp-process-patterns)
  (see matching-processes 0.1)))

(defun flp-kill-processes ()
 (interactive)
 (mapcar (lambda (process-number) (condition-case nil (kill-process process-number) (error nil))) (flp-list-processes)))

(defun flp-running-p-orig ()
 (interactive)
 (and
  (kmax-buffer-exists-p "*WSM*")
  (kmax-buffer-exists-p "*FCMS*")
  (kmax-buffer-exists-p "*ushell*")
  (kmax-buffer-exists-p "*Formalog-REPL*")
  (kmax-buffer-exists-p alexa-skill-flp-buffer-name)
  (shell-command "ps auxwww | grep \"perl ./script/shinycms_server.pl -r -h \" | grep \" -p 3000 -d\" | awk '{print \$2}' | grep -q .")))

(defun flp (&optional load-command)
 ""
 (interactive)
 (if (flp-running-p)
  (error "ERROR: FLP Already running.")
  (progn
   (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/formalog" "*WSM*")
   (sit-for 3.0)
   (ushell)
   (sit-for 1.0)
   (uea-connect)
   (pop-to-buffer "*WSM*")
   (insert (or load-command "./start -u"))
   (comint-send-input)
   (sit-for 3.0)
   (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/formalog/scripts && ./start-repl" "*Formalog-REPL*" nil 'formalog-repl-mode)
   (sit-for 1.0)
   (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/fcms/FCMS && ./begin-fcms" "*FCMS*")
   (sit-for 1.0))))

(defun flp-kill-interactive ()
 ""
 (interactive)
 (if (yes-or-no-p "Kill FLP? ")
  (flp-kill)))

(defun flp-kill ()
 ""
 (interactive)
 (flp-kill-processes)
 (shell-command "killall -9 \"start-repl\"")
 (shell-command "killall -9 \"begin-fcms\"")
 (shell-command "killall -9 \"start-alexa-skill-flp-unilang.sh\"")
 (shell-command (concat "ps auxwww | grep \"perl ./script/shinycms_server.pl -r -h \" | grep \" -p " (flp-get-fcms-port) " -d\" | awk '{print \$2}' | xargs kill -9"))
 (shell-command (concat "ps auxwww | grep \"/usr/lib/jvm/default-java/bin/java -classpath /usr/share/maven/boot/plexus-classworlds-2.x.jar -Dclassworlds.conf=/usr/share/maven/bin/m2.conf -Dmaven.home=/usr/share/maven org.codehaus.plexus.classworlds.launcher.Launcher exec:java -Dexec.executable=”java” -DdisableRequestSignatureCheck=true\" | awk '{print \$2}' | xargs kill -9"))
 
 (kmax-kill-buffer-no-ask (get-buffer "*WSM*"))
 (kmax-kill-buffer-no-ask (get-buffer "*FCMS*"))
 (kmax-kill-buffer-no-ask (get-buffer "*Formalog-REPL*"))
 (kmax-kill-buffer-no-ask (get-buffer "*ushell*"))
 (kmax-kill-buffer-no-ask (get-buffer "*Inform7*"))
 (kmax-kill-buffer-no-ask (get-buffer alexa-skill-flp-buffer-name))
 (if (non-nil inform7-frdcsa-current-package)
  (kmax-kill-buffer-no-ask (get-buffer (inform7-frdcsa-current-story-ni))))
 (cmhsm-stop-cyc)
 (flp-running-p))

(defun flp-restart-old (&optional load-command)
 ""
 (interactive)
 (if (yes-or-no-p "Restart FLP? ")
  (progn
   (flp-kill)
   (flp load-command))))

(defun flp-restart (&optional arg)
 ""
 (interactive "P")
 (if (or arg (yes-or-no-p "Restart FLP? "))
  (progn
   (flp-kill)
   (flp-quick-start))))

;; (defun flp-restart ()
;;  ""
;;  (interactive)
;;  (shell-command "guard-cli --restart"))

(defun flp-get-fcms-port ()
 ""
 (interactive)
 (substring-no-properties
  (prin1-to-string
   (cadr (assoc (kmax-hostname)
	  (list
	   '("ai.frdcsa.org" 3000)
	   '("dev.bytelibrary.org" 3001)
	   '("columcille.frdcsa.org" 3000)
	   '("panoply2.frdcsa.org" 3001)))))))

(defvar flp-predicates nil)

;; (setq flp-predicates nil)
;; (ido-completing-read "Tmp?: " (cdr (nth 1 (nth 0 (list (list "_prolog_list" (list "_prolog_list" "a" "b" "c")))))))

(defun flp-load-predicates ()
 ""
 (if (not (non-nil flp-predicates))
  (progn
   (setq flp-predicates
    (cdr
     (nth 1
      (nth 0
       (formalog-query (list 'var-X) (list "listCompletions" 'var-X)))))))))

(defun flp-complete (flp-line-prefix)
 ""
 (interactive)
 (flp-load-predicates)
 (ido-completing-read "Predicate?: " flp-predicates nil nil flp-line-prefix))

(defun flp-complete-or-tab (&optional arg)
 (interactive "P")
 (let* ((flp-line-prefix (save-excursion
			  (set-mark (point))
			  (beginning-of-line nil)
			  (buffer-substring-no-properties (point) (mark)))))
  (if (string-match "^[\s\t]*$" flp-line-prefix)
   (progn
    ;; (see "yes") 
    (indent-for-tab-command arg))
   (progn
    ;; (see "no")
    (unwind-protect
     (condition-case ex
      (flp-complete flp-line-prefix)
      ('error (indent-for-tab-command arg))))))))

(defun flp-true (&optional proceed-anyway)
 ""
 (interactive)
 (let ((buffer-name "*Free-Life-Planner-true*"))
  (if (and
       (kmax-buffer-exists-p buffer-name)
       (not proceed-anyway))
   (switch-to-buffer buffer-name)
   (run-in-shell
    "cd /var/lib/myfrdcsa/codebases/minor/free-life-planner && ./free-life-planner-true"
    buffer-name
    nil
    'formalog-repl-mode))))

(defun flp-true-restart ()
 ""
 (interactive)
 (let ((buffer-name "*Free-Life-Planner-true*"))
  (if (kmax-buffer-exists-p buffer-name)
   (progn
    (switch-to-buffer buffer-name)
    (end-of-buffer)
    (comint-interrupt-subjob)
    (insert "e")
    (comint-send-input)
    (sit-for 1)
    (flp-true t)
    (formalog-repl-mode))
   (flp-true))))

(defun flp-open-productivity ()
 ""
 (interactive)
 (kmax-find-file-or-create-including-parent-directories "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/sys/flp/autoload/productivity.pl")
 (end-of-buffer)
 (flp-complete-from-predicates-in-current-buffer))

(defun flp-open-incoming ()
 ""
 (interactive)
 (kmax-find-file-or-create-including-parent-directories "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/sys/flp/autoload/incoming.pl")
 (end-of-buffer)
 (flp-complete-from-predicates-in-current-buffer))

(defun flp-open-gourmet ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/internal/gourmet/frdcsa/flp/autoload/pantry_manager.pl"))

(defun flp-open-loan-manager ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/financial-decision-support-system/frdcsa/flp/autoload/loan_manager.pl"))

(defun flp-open-shopping-list ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/resource-manager/shopping_list.pl"))

(defun flp-open-audience-master-agent ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/internal/audience/data-git/agent/master.pl"))

(defun flp-open-audience-agent-snail-mail ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/internal/audience/data-git/agent/snail-mail.pl"))

(defun flp-open-audience-agent-phone ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/internal/audience/data-git/agent/phone.pl"))

(defun flp-open-audience-agent-email ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/internal/audience/data-git/agent/email.pl"))

(defun flp-open-audience-agent-in-person ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/internal/audience/data-git/agent/in-person.pl"))

;; (flp-util-get-approximately-all-predicates-in-current-file "/var/lib/myfrdcsa/codebases/minor/financial-decision-support-system/data-git/loan_manager.pl")

(defun flp-util-get-approximately-all-predicates-in-current-file (file)
 ""
 (freekbs2-util-data-dedumper
  (shell-command-to-string
   (concat "/var/lib/myfrdcsa/codebases/minor/free-life-planner/scripts/list-approximately-all-predicates.pl -f "
    (shell-quote-argument file)))))

(defun flp-complete-from-predicates-in-current-buffer ()
 ""
 (interactive)
 (if flp-complete-from-predicates
  (let ((predicates (flp-util-get-approximately-all-predicates-in-current-file buffer-file-name)))
   (insert (concat (ido-completing-read "Predicate: " predicates) "()."))
   (backward-char 2))))

(defun flp-log (&optional function) ""
 (interactive)
 (if (not (equal nil function))
  (funcall function))
 (end-of-buffer)
 (flp-complete-from-predicates-in-current-buffer))

(defun flp-complete-log () ""
 (interactive)
 (funcall
  (read (ido-completing-read
	 "Please choose function to load: "
	 (apropos-internal "^flp-log-")
	 nil
	 nil
	 "flp-log-"))))

(defun flp-log-productivity ()
 ""
 (interactive)
 (flp-log 'flp-open-productivity))

(defun flp-log-gourmet ()
 ""
 (interactive)
 (flp-log 'flp-open-gourmet))

(defun flp-log-loan-manager ()
 ""
 (interactive)
 (flp-log 'flp-open-loan-manager))

(defun flp-log-shopping-list ()
 ""
 (interactive)
 (flp-log 'flp-open-shopping-list))

(defun flp-log-audience-master-agent ()
 ""
 (interactive)
 (flp-log 'flp-open-audience-master-agent))

(defun flp-log-audience-agent-snail-mail ()
 ""
 (interactive)
 (flp-log 'flp-open-audience-agent-snail-mail))

(defun flp-log-audience-agent-phone ()
 ""
 (interactive)
 (flp-log 'flp-open-audience-agent-phone))

(defun flp-log-audience-agent-email ()
 ""
 (interactive)
 (flp-log 'flp-open-audience-agent-email))

(defun flp-log-audience-agent-in-person ()
 ""
 (interactive)
 (flp-log 'flp-open-audience-agent-in-person))

(defun flp-open-model ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (ido-completing-read "FCMS Model?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/Model | grep '\.pm$'")
    "\n"))))

(defun flp-search-models (&optional search-arg)
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
  (kmax-search-files
   (or search-arg (read-from-minibuffer "Search?: "))
   (kmax-grep-list-regexp
    (split-string
     (shell-command-to-string
      "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/Model | grep '\.pm$'")
     "\n")
    "[^~]$")
   "*FCMS Model Search*"
   ))

(defun flp-open-view ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (ido-completing-read "FCMS View?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/View | grep '\.pm$'")
    "\n"))))

(defun flp-search-views (&optional search-arg)
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
  (kmax-search-files
   (or search-arg (read-from-minibuffer "Search?: "))
   (kmax-grep-list-regexp
    (split-string
     (shell-command-to-string
      "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/View | grep '\.pm$'")
     "\n")
    "[^~]$")
   "*FCMS View Search*"
   ))

(defun flp-open-controller ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (ido-completing-read "FCMS Controller?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/Controller | grep '\.pm$'")
    "\n"))))

(defun flp-search-controllers (&optional search-arg)
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
  (kmax-search-files
   (or search-arg (read-from-minibuffer "Search?: "))
   (kmax-grep-list-regexp
    (split-string
     (shell-command-to-string
      "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/Controller | grep '\.pm$'")
     "\n")
    "[^~]$")
   "*FCMS Controller Search*"
   ))

(defun flp-open-template ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (ido-completing-read "FCMS Template?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/root | grep -v /root/var/lib/myfrdcsa/ | grep '\.tt$'")
    "\n"))))

(defun flp-search-templates (&optional search-arg)
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
  (kmax-search-files
   (or search-arg (read-from-minibuffer "Search?: "))
   (kmax-grep-list-regexp
    (split-string
     (shell-command-to-string
      "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/root/ -not -path var | grep -E '\.tt2?$'")
     "\n")
    "[^~]$")
   "*FCMS Template Search*"
  ))

(defun flp-open-frdcsa-sys-flp-file ()
 (interactive)
 (let* ((chosen-directory (radar-select-directory radar-radar-dirs))
	(basename (eshell/basename chosen-directory))
	(chosen (concat (join "_" (split-string basename "-")) ".pl")))
  (ffap (frdcsa-el-concat-dir (list chosen-directory "frdcsa" "sys" "flp" chosen)))))

(defun flp-open-frdcsa-sys-flp-autoload-file ()
 (interactive)
 (let* ((chosen-directory (radar-select-directory radar-radar-dirs))
	(dir (frdcsa-el-concat-dir (list chosen-directory "frdcsa" "sys" "flp" "autoload"))))
  (kmax-mkdir-p dir)
  (dired dir)))

(defun flp-quick-start ()
 ""
 (interactive)
 (if (flp-running-p)
  (error "ERROR: FLP Already running.")
  (progn
   (flp)
   (cmhsm-cyc-quick-start)
   (inform7-frdcsa-quick-start)
   (alexa-skill-flp-quick-start)
   (cmhsm-temporary-connect-system-cyc)
   (flp-set-windows-with-additionals t)
   (flp-running-p))))

(defun flp-open-todos (&optional arg)
 ""
 (interactive "P")
 ;; (kmax-assert-not-a-dedicated-emacs-session)
 (delete-other-windows)
 (if arg
  (ffap flp-latest-file)
  (ffap flp-todo-file))
 (split-window-right)
 (other-window 1)
 (radar-edit-latest-symlink-dated-daily-todo-file)
 (if arg
  (ffap flp-todo-file)
  (ffap flp-latest-file))
 (if
  (not
   (string=
    (kmax-chase (radar-get-latest-dated-daily-todo-file))
    (kmax-chase (radar-get-latest-symlink-dated-daily-todo-file))))
  (progn
   (split-window-below)
   (other-window 1)
   (radar-edit-latest-dated-daily-todo-file)
   (other-window 1))
  (other-window 1)))

(defun flp-open-cycs ()
 ""
 (interactive)
 (delete-other-windows)
 (ffap flp-todo-file)
 (split-window-right)
 (split-window-below)
 (other-window 1)
 (radar-edit-latest-symlink-dated-daily-todo-file)
 (ffap flp-latest-file)
 ;; (if
 ;;  (not
 ;;   (string=
 ;;    (kmax-chase (radar-get-latest-dated-daily-todo-file))
 ;;    (kmax-chase (radar-get-latest-symlink-dated-daily-todo-file))))
 ;;  (progn
 ;;   (split-window-below)
 ;;   (other-window 1)
 ;;   (radar-edit-latest-dated-daily-todo-file)
 ;;   (other-window 1))
 ;;  (other-window 1))
 (other-window 1)
 (split-window-below)
 (split-window-below)
 (switch-to-buffer (get-buffer "*Cyc-10 on vagrant@ai.frdcsa.org:2241*"))
 (other-window 1)
 (switch-to-buffer (get-buffer "*ResearchCyc-1.0 on vagrant@kbs32.frdcsa.org:2238*"))
 (other-window 1)
 (switch-to-buffer (get-buffer "*ResearchCyc-4.0q*")))

(defun flp-search-source-file-names (&optional search-arg)
 ""
 (interactive)
 (flp-load-source-files)
 (let* ((search (or search-arg (read-from-minibuffer "Source File Search: "))))
  (see (kmax-grep-list-regexp (flp-get-actual-source-files) search))))

(defun flp-open-akahige ()
 ""
 (interactive)
 (akahige))

(defun flp-open-verber ()
 ""
 (interactive)
 (verber-run-verber))

(defun flp-open-verber-edit-domains-new ()
 ""
 (interactive)
 (see
  (formalog-query (list 'var-Type 'var-Name 'var-Filename) (list "hasCustomPDDL" 'var-Type 'var-Name 'var-Filename))))

(defun flp-open-verber-edit-domains ()
 ""
 (interactive)
 (diamond-emacs-open-files
  (list
   "/var/lib/myfrdcsa/codebases/minor/resource-manager/billpayment.d.verb"
   "/var/lib/myfrdcsa/codebases/minor/resource-manager/billpayment.p.verb"
   "/var/lib/myfrdcsa/repositories/cvs/sigmakee/KBs/FinancialOntology.kif"
   "~/to.do"
   )))

(defun flp-open-verber-run-verber ()
 ""
 (interactive))

;; hasCustomPDDL(domain,billPayment,'/var/lib/myfrdcsa/codebases/minor/resource-manager/bill_payment.d.pddl').
;; hasCustomPDDL(problem,billPayment,'/var/lib/myfrdcsa/codebases/minor/resource-manager/bill_payment.p.pddl').

;; hasCustomPDDL(domain,medications,'/var/lib/myfrdcsa/codebases/internal/akahige/medications.d.pddl').
;; hasCustomPDDL(problem,medications,'/var/lib/myfrdcsa/codebases/internal/akahige/medications.p.pddl').

;; (("_prolog_list" "domain" "billPayment" "/var/lib/myfrdcsa/codebases/minor/resource-manager/bill_payment.d.pddl") ("_prolog_list" "problem" "billPayment" "/var/lib/myfrdcsa/codebases/minor/resource-manager/bill_payment.p.pddl") ("_prolog_list" "domain" "medications" "/var/lib/myfrdcsa/codebases/internal/akahige/medications.d.pddl") ("_prolog_list" "problem" "medications" "/var/lib/myfrdcsa/codebases/internal/akahige/medications.p.pddl"))

(defun flp-list-errors ()
 ""
 (interactive)
 (assert (flp-running-p))
 (save-excursion
  (pop-to-buffer (get-buffer "*WSM*"))
  (let* ((errors
	  (kmax-grep-list-regexp (split-string (kmax-buffer-contents) "\n") "ERROR" nil))
	 (buffer "*FLP Errors*"))
   (if (kmax-buffer-exists-p buffer)
    (kmax-kill-buffer-no-ask buffer))
   (get-buffer-create buffer)
   (switch-to-buffer buffer)
   (insert (join "\n" errors)))))

;;

(defun flp-list-multifile-and-dynamic ()
 ""
 (interactive)
 (assert (flp-running-p))
 (save-excursion
  (pop-to-buffer (get-buffer "*WSM*"))
  (let* ((filename
	  "/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/multifile-and-dynamic-directives.pl")
	 (regexp "^:- multifile \\([[:ascii:]]+\\).\n\n")
	 (list1
	  (kmax-grep-list-regexp (split-string (kmax-buffer-contents) "\n") "Use :- discontiguous \\(.+?\\). to suppress this message" nil))
	 (list2
	  (kmax-grep-list-regexp (split-string (kmax-buffer-contents) "\n") "Redefined static procedure \\(.+?\\)$" nil))
	 (discontiguous
	  (sort
	   (-uniq
	    (append
	     (mapcar (lambda (item)
		      (if (string-match "Use :- discontiguous \\(.+?\\). to suppress this message" item)
		       (match-string 1 item)))
	      list1)
	     (mapcar (lambda (item)
		      (if (string-match "Redefined static procedure \\(.+?\\)$" item)
		       (match-string 1 item)))
	      list2)))
	    #'string-lessp))
	 (multifile-and-dynamic-directives-file-contents
	  (kmax-file-contents filename))
	 (existing-multifile-directive-contents
	  (split-string
	   (progn
	    (string-match
	     regexp
	     multifile-and-dynamic-directives-file-contents)
	    (match-string 1 multifile-and-dynamic-directives-file-contents))
	   "[ \n]*,[ \n]+"))
	 (replacement-string
	  (concat ":- multifile "
	   (join
	    ", "
	    (sort
	     (cl-nunion discontiguous existing-multifile-directive-contents)
	     #'string-lessp))
	   ".\n\n")))
   (ffap filename)
   (replace-regexp regexp replacement-string)
   (previous-line 2)
   (beginning-of-line)
   (prolog-fill-paragraph))))

(defun flp-list-unimplemented-yet-referenced-predicates ()
 ""
 (interactive)
 (assert (flp-running-p))
 (save-excursion
  (pop-to-buffer (get-buffer "*WSM*"))
  (let* ((contents (kmax-buffer-contents))
	 (regexp "% Restarting analysis ...\\([[:ascii:]]+\\)print_result")
	 (matches
	  (progn
	   (string-match regexp contents)
	   (match-string 1 contents))))
   (see matches))))

(defun flp-edit-multifile-and-dynamic-file ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/multifile-and-dynamic-directives.pl"))

;; (let ((temp "hello hello"))
;;  (string-match "^he\\([.\n]+\\)lo$" temp)
;;  (message (prin1-to-string (match-string 1 temp))))

;; (let ((temp "hello 
;; hello"))
;;  (string-match "^he\\([.\n]+\\)lo$" temp)
;;  (message (prin1-to-string (match-string 1 temp))))

;; (let ((multifile-and-dynamic-directives-file-contents
;; 	  (kmax-file-contents
;; 	   "/var/lib/myfrdcsa/codebases/minor/free-life-planner/lib/multifile-and-dynamic-directives.pl")))
;;  (string-match "^:- multifile \\([[:ascii:]]+\\)\n\n" multifile-and-dynamic-directives-file-contents)
;;  (message (prin1-to-string (match-string 1 multifile-and-dynamic-directives-file-contents))))

(defun flp-current-buffer-flp-source-file-p ()
 ""
 (interactive)
 (see (flp-source-file-p buffer-file-name)))

(defun flp-source-file-p (file)
 ""
 (element
  (kmax-chase file)
  (mapcar #'cdr flp-source-files-chase-alist)))

(defun flp-source-file-p-orig (file)
 ""
 (element
  (kmax-chase file)
  (mapcar #'kmax-chase
   (kmax-grep-list flp-source-files #'stringp))))

(defun element (element set)
 (cl-subsetp (list element) set :test 'equal))

(defun flp-true-reload-current-source-file ()
 ""
 (interactive)
 (let ((filename buffer-file-name))
  (flp-true)
  (kmax-run-command-in-repl (concat "consult('" filename "')."))))

(defun flp-reload-all-modified-source-files ()
 ""
 (interactive)
 (formalog-query
  nil
  (list "make")))

(defun flp-reload-current-source-file ()
 ""
 (interactive)
 (assert (flp-source-file-p buffer-file-name))
 (let* ((filename buffer-file-name)
	(result
	  (formalog-query
	   (list 'var-MainType)
	   (list "getMainTypeForFilename" 'var-MainType filename)))
	(type (cadar result)))
  (see (list result filename))
  (if (kmax-non-empty-string type)
   (formalog-query
    nil
    (list (concat "prolog_" type) filename)))))

(defun flp-edit-speech-dialog-interface-questions-list ()
 (interactive)
 (kmax-edit-list
  'elisp
  "free-life-planner-speech-interface-questions"
  "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/emacs/free-life-planner-speech-interface-questions.el"))

(defun flp-create-new-flp-source-file ()
 ""
 (interactive)
 ;; need to classify its purpse.  records it's purpose in a statement
 ;; in it.  And since we now know how to compute where those
 ;; statements are, that's important.
 )

(defun flp-constant-apropos-completing-read (&optional search-arg)
 (interactive)
 (freekbs2-push-onto-stack
  (ido-completing-read
   "Constant apropos completions: "
   (flp-constant-apropos search-arg))))

(defun flp-constant-apropos (&optional search-arg)
 (interactive)
 (mapcar
  #'cadr
  (formalog-query
   (list 'var-Y)
   (list
    "constant_apropos"
    (or search-arg (read-from-minibuffer "String for constant apropos?: ")) 'var-Y))))

(defun flp-restart-fcms ()
 ""
 (interactive)
 (kmax-not-yet-implemented))

(defun flp-list-errors-with-current-file ()
 ""
 (interactive)
 ;; (assert (flp-running-p))
 (save-excursion
  ;; (pop-to-buffer (get-buffer "*WSM*"))
  (let ((filename buffer-file-name))
   (if (kmax-file-exists-p filename)
    (progn
     (run-in-shell
      (concat
       "swipl -s "
       (shell-quote-argument filename))
      "*FLP File SWIPL*"
      nil 'formalog-repl-mode)
     (sit-for 3)
     (let* ((errors
	     (kmax-grep-list-regexp (split-string (kmax-buffer-contents) "\n") "ERROR" nil))
	    (buffer "*FLP File Errors*"))
      (if (kmax-buffer-exists-p buffer)
       (kmax-kill-buffer-no-ask buffer))
      (get-buffer-create buffer)
      (switch-to-buffer buffer)
      (insert (join "\n" errors))))))))

(defun flp-kill-wsm-process ()
 ""
 (interactive)
 (shell-command "ps auxwww | grep  \"/usr/bin/perl -w ./start -u\" | grep -v grep | awk '{print $2}' | xargs kill -9"))

(defun flp-emacs-client-fix ()
 ""
 (interactive)
 (pop-to-buffer "*ushell*")
 (insert "UniLang, deregister Emacs-Client")
 (eshell-send-input)
 (sit-for 1)
 (uea-connect))

;; (jump-to-directory /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/root)
;; (jump-to-directory /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib)

(add-to-list 'load-path "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/emacs")
(require 'free-life-planner-demo)
;; (see "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/emacs/free-life-planner-demo.el")
(require 'free-life-planner-planning-scheduling-and-execution)
;; (see "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/emacs/free-life-planner-planning-scheduling-and-execution.el")
(require 'free-life-planner-speech-interface-questions)
;; (see "/var/lib/myfrdcsa/codebases/minor/free-life-planner/frdcsa/emacs/free-life-planner-speech-interface-questions.el")

(add-to-list 'load-path "/var/lib/myfrdcsa/codebases/minor/free-life-planner/projects/swipl-emacs")
(require 'swipl-emacs)
;; (see "/var/lib/myfrdcsa/codebases/minor/free-life-planner/projects/swipl-emacs/swipl-emacs.el")


(provide 'free-life-planner)
