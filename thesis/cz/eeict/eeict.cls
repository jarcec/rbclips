%%
%% This is file `eeict.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% Author: Martin Drahansky, drahan@fit.vutbr.cz
%%
%% eeict.dtx  (with options: `class')
%% 
%% 

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{eeict}% 
    [2002/11/11 Dokumentarni trida pro prispevky do sborniku EEICT]
\usepackage[T1]{fontenc} 
\LoadClass[a4paper,onecolumn,12pt]{article} 
\setlength{\hoffset}{-0.9cm} 
\setlength{\voffset}{-1.9cm} 
\setlength{\textheight}{24.0cm} 
\setlength{\textwidth}{16.0cm} 
\setlength{\parskip}{6pt}
\usepackage{mathptm} 
\renewcommand\rmdefault{ptm} 
\renewcommand\ttdefault{pcr} 

\ifx\pdfoutput\undefined  % nejedeme pod pdftexem
  \usepackage[czech,english]{babel}
  \main@language{english}
  \usepackage[latin2]{inputenc}
  \usepackage{graphics}
  \usepackage{epsfig}
\else % je to pdftex !
  \usepackage[czech,english]{babel}
  \main@language{english}
  \usepackage[latin2]{inputenc}
  \usepackage[pdftex]{graphicx}
\fi

\pagestyle{empty}
\setlength{\parindent}{0cm} 
\let\@afterindentfalse\@afterindenttrue 
\@afterindenttrue 

\newcommand\supervisor[1]{\def\@skolitel{Supervised by: #1}}
\newcommand\programme[1]{\def\@program{#1}}
\newcommand\emails[1]{\def\@emajls{E-mail: #1}}
\newcommand\emailv[1]{\def\@emajlv{E-mail: #1}}
\renewcommand\maketitle{
\begin{center}
  \vspace*{8mm}
  {\Large\textbf{\MakeUppercase{\@title}} }\\
  \vspace{9mm}
  {\normalsize\textbf\@author}\\
  \vspace{0.01mm}
  {\footnotesize\@program}\\
  \vspace{0.01mm}
  {\footnotesize\@emajls}\\
[5mm]
  \@skolitel\\
  \vspace{0.01mm}
  {\footnotesize\@emajlv}
  \vspace{-3mm}
  \hspace{0.8mm}
\end{center}} 

\renewcommand\section{\@startsection{section}{1}{-0.61cm}%
                                   {-3.5ex \@plus -1ex \@minus -.2ex}%
                                   {0.2ex \@plus.2ex}%
                                   {\normalfont\normalsize\bfseries\MakeUppercase}}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}% 
                                     {-2.25ex\@plus -1ex \@minus -.2ex}%
                                     {0.2ex \@plus .2ex}%
                                     {\normalfont\normalsize\bfseries\MakeUppercase}}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                                     {-2.25ex\@plus -1ex \@minus -.2ex}%
                                     {0.2ex \@plus .2ex}%
                                     {\normalfont\normalsize\MakeUppercase}}
\renewcommand\abstract[1]{
  \section*{\abstractname}
  {#1}
}
  \def\sktitle{Supervised by: }
  \def\@skolitel{the work has no suppervisor}
\def\uv#1{\iflanguage{english}{``#1''}%
                              {\quotedblbase #1\textquotedblleft}}%
\catcode`\"=\active
\def"{\bgroup
    \iflanguage{english}%
        {``\def"{''\egroup}}%
        {\quotedblbase\def"{\textquotedblleft\egroup}}}
\endinput
%%
%% End of file `eeict.cls'.
