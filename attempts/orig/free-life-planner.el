;; (add-hook 'shell-mode-hook (lambda () (local-unset-key "\C-c\C-e")))

(global-set-key "\C-cffit" 'free-life-planner-insert-prolog-timestamp)
(global-set-key "\C-cfff" 'free-life-planner-load-tasks)
(global-set-key "\C-cffx" 'free-life-planner)
(global-set-key "\C-cffk" 'free-life-planner-kill)
(global-set-key "\C-cffr" 'free-life-planner-restart)

(defun free-life-planner-insert-prolog-timestamp ()
 ""
 (interactive)
 (insert (free-life-planner-prolog-timestamp)))

(defun free-life-planner-prolog-timestamp ()
 (chomp (shell-command-to-string "date \"+[%Y-%m-%d,%H:%M:%S]\"")))

(defun free-life-planner-load-tasks ()
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/free-life-planner/data-git/systems/planning/tasks.pl")
 (end-of-buffer))

(defun free-life-planner ()
 ""
 (interactive)
 (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/formalog" "*WSM*")
 (sit-for 3)
 (ushell)
 (sit-for 1)
 (save-excursion
  (pop-to-buffer "*WSM*")
  (insert "./start -u")
  (comint-send-input))
 (sit-for 3.0)
 (run-in-shell "cd /var/lib/myfrdcsa/codebases/minor/fcms/FCMS && ./begin-fcms" "*FCMS*"))

(defun free-life-planner-kill-fcms ()
 ""
 (interactive)
 (shell-command "killall -9 begin-fcms")
 (shell-command "ps auxwww | grep \"perl ./script/shinycms_server.pl -r -h <IP> -p 3000 -d\" | awk '{print \$2}' | xargs kill -9")
 (kill-buffer (get-buffer "*FCMS*")))

(defun free-life-planner-kill ()
 ""
 (interactive)
 (kill-buffer (get-buffer "*WSM*"))
 (kill-buffer (get-buffer "*ushell*"))
 (free-life-planner-kill-fcms))


(defun free-life-planner-restart ()
 ""
 (interactive)
 (shell-command "guard-cli --restart"))
