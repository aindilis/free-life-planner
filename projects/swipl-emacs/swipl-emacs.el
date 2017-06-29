;; prolog.el
(autoload 'run-prolog "prolog" "Start a Prolog sub-process." t)
(autoload 'prolog-mode "prolog" "Major mode for editing Prolog programs." t)
(autoload 'mercury-mode "prolog" "Major mode for editing Mercury programs." t)
(setq prolog-system 'swi)
;; (setq auto-mode-alist (append '(("\\.pl$" . prolog-mode)
;;                                 ("\\.m$" . mercury-mode))
;; 		       auto-mode-alist))

;; ediprolog.el
(require 'ediprolog)
;; (global-set-key [f10] 'ediprolog-dwim)
;; (global-set-key "\C-c\C-e" 'ediprolog-dwim)

(setq prolog-electric-if-then-else-flag t)

;; etrace.el
(load "/var/lib/myfrdcsa/codebases/minor/free-life-planner/projects/swipl-emacs/etrace.el")

(provide 'swipl-emacs)

