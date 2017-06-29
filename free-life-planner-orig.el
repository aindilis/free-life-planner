;; (add-hook 'shell-mode-hook (lambda () (local-unset-key "\C-c\C-e")))

(global-set-key "\C-c\C-k\C-vFLP" 'flp-quick-start)
(global-set-key "\C-c\C-k\C-vLI" 'flp-quick-start)
(global-set-key "\C-c\C-k\C-kFLP" 'flp-quick-stop)
(global-set-key "\C-c\C-k\C-kLI" 'flp-quick-stop)
;; (global-set-key "\C-c\C-k\C-rLI" 'flp-quick-restart)

(global-set-key "\C-cffe" 'flp-edit-flp-elisp)
(global-set-key "\C-cffcc" 'flp-open-controller)
(global-set-key "\C-cffcm" 'flp-open-model)
(global-set-key "\C-cffct" 'flp-open-template)
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
(global-set-key "\C-cffk" 'flp-kill-interactive)
(global-set-key "\C-cffr" 'flp-restart)
(global-set-key "\C-cffn" 'flp-new-task)
(global-set-key "\C-ceS" 'flp-free-life-planner-true)
(global-set-key "\C-cesr" 'flp-free-life-planner-true-restart)
;; (global-set-key [?\C-\t] 'flp-complete-or-tab)
;; (global-set-key (kbd "<C-tab>") 'flp-complete-or-tab)
;; (global-set-key (kbd "<tab>") 'flp-complete-or-tab)
(global-set-key "\C-cfft" 'flp-complete-or-tab)
(global-set-key "\C-cffTp" 'perllib-perl-or-prolog-mode)

(global-set-key "\C-cffogs" 'flp-grep-source-files)
(global-set-key "\C-cffogS" 'flp-grep-source-file-names)
(global-set-key "\C-cffogP" 'flp-grep-source-files-reload)
(global-set-key "\C-cffop" 'flp-open-source-file)
(global-set-key "\C-cffoP" 'flp-open-source-file-reload)
(global-set-key "\C-cffol" 'flp-list-source-files)
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
(global-set-key "\C-cfflama" 'flp-log-audience-master-agent)
(global-set-key "\C-cfflasm" 'flp-log-audience-agent-snail-mail)
(global-set-key "\C-cfflaph" 'flp-log-audience-agent-phone)
(global-set-key "\C-cfflaem" 'flp-log-audience-agent-email)
(global-set-key "\C-cfflaip" 'flp-log-audience-agent-in-person)



;; (defvar fcms-port-values ai.frdcsa.org, 3000, panoply2.frdcsa.org, 3001)

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

(defun flp-load-source-files ()
 ""
 (if (not (non-nil flp-source-files))
  (progn
   (setq flp-source-files
    (cdr
     (nth 1
      (nth 0
       (formalog-query (list 'var-X) (list "listFiles" 'var-X))))))
   (shift flp-source-files))))

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
   (concat "tr '\\n' '\\0' <" file-containing-files-to-search " | LANG=C xargs -r0 grep -F " (shell-quote-argument search))
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
 (let ((file (completing-read "Source File: " (flp-get-actual-source-files))))
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

(defun flp-set-windows-with-additionals (&optional arg)
 (interactive "P")
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
  (kmax-map-end-of-buffer-and-recenter-top-bottom-over-all-visible-buffers (list flp-latest-file))))

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

(defun flp-running-p ()
 (interactive)
 (and
  (gnus-buffer-exists-p "*WSM*")
  (gnus-buffer-exists-p "*FCMS*")
  (gnus-buffer-exists-p "*ushell*")
  (gnus-buffer-exists-p "*Formalog-REPL*")
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
   (pop-to-buffer "*WSM*")
   (insert (or load-command "./start -u"))
   (comint-send-input)
   (sit-for 3.0)
   (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/formalog/scripts && ./start-repl" "*Formalog-REPL*" nil 'formalog-repl-mode)
   (sit-for 1.0)
   (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/fcms/FCMS && ./begin-fcms" "*FCMS*")
   (sit-for 1.0)
   (flp-set-windows-with-additionals))))


;; (defun flp-kill-fcms ()
;;  ""
;;  (interactive)
;;  (shell-command "killall -9 begin-fcms")
;;  (shell-command "ps auxwww | grep \"perl ./script/shinycms_server.pl -r -h \" | grep \" -p 3000 -d\" | awk '{print \$2}' | xargs kill -9")
;;  (kmax-kill-buffer-no-ask (get-buffer "*FCMS*")))

(defun flp-kill-interactive ()
 ""
 (interactive)
 (if (yes-or-no-p "Kill FLP? ")
  (flp-kill)))

(defun flp-kill ()
 ""
 (shell-command "killall -9 \"start-repl\"")
 (kmax-kill-buffer-no-ask (get-buffer "*WSM*"))
 (kmax-kill-buffer-no-ask (get-buffer "*Formalog-REPL*"))
 (kmax-kill-buffer-no-ask (get-buffer "*ushell*"))
 (flp-kill-fcms))

(defun flp-restart (&optional load-command)
 ""
 (interactive)
 (if (yes-or-no-p "Restart FLP? ")
  (progn
   (flp-kill)
   (flp load-command))))
 
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

(defun flp-kill-fcms ()
 ""
 (interactive)
 (shell-command "killall -9 start-repl")
 (shell-command "killall -9 begin-fcms")
 (shell-command (concat "ps auxwww | grep \"perl ./script/shinycms_server.pl -r -h \" | grep \" -p " (flp-get-fcms-port) " -d\" | awk '{print \$2}' | xargs kill -9"))
 (kmax-kill-buffer-no-ask (get-buffer "*FCMS*")))

(defvar flp-predicates nil)

;; (setq flp-predicates nil)
;; (completing-read "Tmp?: " (cdr (nth 1 (nth 0 (list (list "_prolog_list" (list "_prolog_list" "a" "b" "c")))))))

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
 (completing-read "Predicate?: " flp-predicates nil nil flp-line-prefix))

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

(defun flp-free-life-planner-true (&optional proceed-anyway)
 ""
 (interactive)
 (let ((buffer-name "*Free-Life-Planner-true*"))
  (if (and
       (gnus-buffer-exists-p buffer-name)
       (not proceed-anyway))
   (switch-to-buffer buffer-name)
   (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/free-life-planner && ./free-life-planner-true" buffer-name))))

(defun flp-free-life-planner-true-restart ()
 ""
 (interactive)
 (let ((buffer-name "*Free-Life-Planner-true*"))
  (switch-to-buffer buffer-name)
  (end-of-buffer)
  (comint-interrupt-subjob)
  (insert "e")
  (comint-send-input)
  (sit-for 1)
  (flp-free-life-planner-true t)))

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
 (let ((predicates (flp-util-get-approximately-all-predicates-in-current-file buffer-file-name)))
    (insert (concat (completing-read "Predicate: " predicates) "()."))
    (backward-char 2)))

(defun flp-log (&optional function) ""
 (interactive)
 (if (not (equal nil function))
  (funcall function))
 (end-of-buffer)
 (flp-complete-from-predicates-in-current-buffer))

(defun flp-complete-log () ""
 (interactive)
 (funcall
  (read (completing-read
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

(defun flp-open-controller ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (completing-read "FCMS Controller?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/Controller | grep '\.pm$'")
    "\n"))))

(defun flp-open-model ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (completing-read "FCMS Controller?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/lib/ShinyCMS/Model | grep '\.pm$'")
    "\n"))))

(defun flp-open-template ()
 ""
 (interactive)
 ;; FIXME: make this so that it searches the basename, with a hash to
 ;; the fullname
 (ffap
  (completing-read "FCMS Template?: "
   (split-string
    (shell-command-to-string
     "find /var/lib/myfrdcsa/codebases/minor/fcms/FCMS/root | grep -v /root/var/lib/myfrdcsa/ | grep '\.tt$'")
    "\n"))))


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
 (flp)
 (cmhsm-cyc-quick-start)
 (inform7-frdcsa-quick-start))

(defun flp-quick-stop ()
 ""
 (interactive)
 (if (yes-or-no-p "Kill FLP Quick Start? ")
  (progn
   (kmax-kill-buffer-no-ask (get-buffer (cmhsm-current-connection-fn)))
   (kmax-kill-buffer-no-ask (get-buffer "*Inform7*"))
   (kmax-kill-buffer-no-ask (get-buffer (inform7-frdcsa-current-story-ni)))
   (flp-kill))))

(provide 'free-life-planner)
