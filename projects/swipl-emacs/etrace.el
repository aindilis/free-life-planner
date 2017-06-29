;;; etrace.el --- Graphical Prolog Tracer in Emacs

;; Copyright (C) 2007  Markus Triska

;; Author: Markus Triska <markus.triska@gmx.at>
;; Keywords: languages, processes

;; Public domain code.

;;; Commentary:

;; Graphical tracing of Prolog in Emacs.

;; Copy etrace.el to your home directory, and add to your .emacs:

;;    (load "~/etrace")

;; Do M-x etrace and enter a query to start tracing a goal in the file
;; you are editing. This currently works with SWI Prolog (it uses the
;; switches -g and -s); other Prolog implementations might require
;; slight modifications. If the tracer's output contains a string of
;; the form ``etrace("File", Start, End, Style)'', then the relevant
;; portion of File is displayed, and positions Start+1 to End+1 are
;; coloured depending on Style (red for ``fail'', blue for ``exit'',
;; green otherwise).  Include the Prolog library "etrace" in your
;; programs to augment the tracer's output with such strings.

;;; Code:

(require 'comint)

(defconst etrace-version "0.5")

(defgroup etrace nil
  "Trace Prolog with Emacs"
  :group 'languages
  :group 'processes)

;;;###autoload
(defcustom etrace-program "swipl"
  "Program name of the Prolog executable."
  :group 'etrace
  :type 'string)

;;;###autoload
(defcustom etrace-program-switches '("-q")
  "List of switches passed to the Prolog process."
  :group 'etrace
  :type 'sexp)

(defcustom etrace-show-position-strings nil
  "Whether positional information emitted by the tracer in the
form etrace(...) is shown."
  :group 'etrace
  :type 'boolean)

(defvar etrace-overlay nil)
(defvar etrace-query-history nil)

(unless (fboundp 'with-selected-window)
  (defmacro with-selected-window (window &rest body)
    "Rudimentary compatibility definition for Emacs < 22."
    `(let ((old-selected-window (selected-window)))
       (save-current-buffer
	 (select-window ,window)
	 ,@body)
       (select-window old-selected-window))))

(defun etrace-filter (str)
  "This filter works on the Prolog process output. It looks for
``etrace(\"File\", Start, End, Style).'' substrings, eliminates
them if requested, and performs highlighting."
  (when (string-match
	 (concat "\\(?:.\\|\n\\)*etrace(\"\\(.*?\\)\", \\([0-9]+\\), "
		 "\\([0-9]+\\), \\(.*?\\))") str)
    (let ((file (match-string 1 str))
	  (start (string-to-number (match-string 2 str)))
	  (end (string-to-number (match-string 3 str)))
	  (style (match-string 4 str))
	  buf)
      (when (file-regular-p file)
	(setq buf (find-file-noselect file))
	(with-selected-window (display-buffer buf)
	  (etrace-unhighlight)
	  (setq etrace-overlay (make-overlay (1+ start) (+ 1 end)))
	  (overlay-put etrace-overlay 'face
		       (cond ((string= style "fail") '((:background "red")))
			     ((string= style "exit") '((:background "blue")))
			     (t '((:background "green")))))
	  (goto-char (1+ start))))))
  (let ((ret str))
    (unless etrace-show-position-strings
      (while (string-match (concat
			    "\\(\\(?:.\\|\n\\)*?\\)etrace(\".*?\", [0-9]+, "
			    "[0-9]+, .*?)\n\\(\\(?:.\\|\n\\)*\\)") ret)
	(setq ret (concat (match-string 1 ret) (match-string 2 ret)))))
    ret))

(defun etrace-unhighlight ()
  "Remove any highlighting left by etrace."
  (interactive)
  (if (overlayp etrace-overlay)
      (delete-overlay etrace-overlay)))

;;;###autoload
(defun etrace-attach (&optional local)
  "Advise comint to handle ``etrace(...).'' strings.
If LOCAL is non-nil, restricted to the comint process in the
current buffer. Useful in shells and inferior Prolog processes."
  (interactive "P")
  (add-hook 'comint-preoutput-filter-functions 'etrace-filter nil local)
  ; Emacs 21 workaround for "t" appearing in preoutput-filters
  (if (< emacs-major-version 22)
      (setq comint-preoutput-filter-functions '(etrace-filter)))
  (add-hook 'kill-buffer-hook 'etrace-unhighlight nil local))

(defun etrace-detach (&optional local)
  "Remove tracing feature from comint."
  (interactive "P")
  (remove-hook 'comint-preoutput-filter-functions 'etrace-filter local)
  (etrace-unhighlight))

;;;###autoload
(defun etrace (&optional arg)
  "Ask for a goal and trace its execution.
If ARG is non-nil, ask for a file name. Otherwise, use the file
in the current buffer."
  (interactive "P")
  (let ((file (buffer-file-name)))
    (if (or arg (not file))
	(setq file (read-file-name "File: ")))
    (let ((query (read-string "Query: ?- " nil 'etrace-query-history)))
      (let ((proc (get-buffer-process "*etrace*")))
	(when (and proc (eq (process-status proc) 'run))
	    (kill-process proc)
	    (kill-buffer "*etrace*")
	    (sit-for 0.1)))
      (apply 'make-comint "etrace" etrace-program nil
	     "-g" (concat "trace, " query)
	     "-s" (expand-file-name file) etrace-program-switches)
      (pop-to-buffer "*etrace*")
      (etrace-attach t))))

;;;###autoload
(defun etrace-version ()
  (interactive)
  (message "Using version %s of etrace" etrace-version))

(provide 'etrace)
;;; etrace.el ends here
