(global-set-key "\C-c\C-k\C-vFLD" 'flp-demo-quick-start)
(global-set-key "\C-cffds" 'flp-demo-set-windows)

(defun flp-demo-quick-start ()
 ""
 (interactive)
 (3t-frdcsa-plan-restart-lad)
 (flp-demo-rte)
 (do-convert-start)
 (flp-demo-set-windows))

(defun flp-demo-set-windows ()
 ""
 (interactive)
 (delete-other-windows)
 (switch-to-buffer "*3t Planner*")
 (split-window-right)
 (split-window-below)
 (other-window 1)
 (switch-to-buffer "*RTE*")
 (other-window 1)
 (switch-to-buffer "*Do Convert*"))

(defun flp-demo-rte ()
 ""
 (interactive)
 (unless (kmax-buffer-exists-p "*RTE*")
  (run-in-shell "run-rte" "*RTE*"))
 ;; (shell-command-to-string "/var/lib/myfrdcsa/codebases/minor/rte/scripts/list-results.pl")
 )

(defun flp-demo-set-windows ())

(provide 'free-life-planner-demo)

