# bash completion for GNU make

# This is for development only. It's normally defined inside bash-completion.
have() { return 1; }


_scons()
{
    local file sconsf sconsf_dir="." sconsf_inc cur prev i split=false

    COMPREPLY=()
    _get_comp_words_by_ref cur prev

    _split_longopt && split=true

    case $prev in
        --cache-debug|-f|--file|--makefile|--sconstruct|--profile|--site-dir|--taskmastertrace)
            _filedir
            return 0
            ;;
        -C|--directory|-I|--include-dir|--site-dir|-Y|--repository|--srcdir)
            _filedir -d
            return 0
            ;;
    esac

    $split && return 0
    
    # scons -H | awk 'BEGIN{ FS="[ =,]+";}{ for(i = 1; i < NF; i++){ if(match($i, "-")){ printf("%s, ", $i)} } } END{ print ""; }'
    
    if [[ "$cur" == -* ]]; then
        COMPREPLY=( $( compgen -W '-b, -d, -e, -m, -S, -t, -w, --environment-overrides, --no-keep-going, --no-print-directory, --print-directory, --stop, -c, --clean, --remove, -C, --directory, --cache-debug, --cache-disable, --cache-force, already-built, --cache-show, --config, -D, --debug, --diskcheck, on-disk, --duplicate, hard-soft-copy, soft-hard-copy, hard-copy, soft-copy, -f, --file, --makefile, --sconstruct, top-level, -h, --help, -H, --help-options, -i, --ignore-errors, -I, --include-dir, --implicit-cache, --implicit-deps-changed, --implicit-deps-unchanged, --interact, --interactive, -j, --jobs, -k, --keep-going, --max-drift, --md5-chunksize, chunk-size, -n, --no-exec, --just-print, --dry-run, --no-site-dir, --profile, -q, --question, -Q, --random, -s, --silent, --quiet, --site-dir, --stack-size, --taskmastertrace, --tree, -u, --up, --search-up, -U, -v, --version, --warn, WARNING-SPEC, --warning, -Y, --repository, --srcdir' -- "$cur" ) )
    else
        # before we check for sconsfiles, see if a path was specified
        # with -C/--directory
        for (( i=0; i < ${#COMP_WORDS[@]}; i++ )); do
            if [[ ${COMP_WORDS[i]} == -@(C|-directory) ]]; then
                # eval for tilde expansion
                eval sconsf_dir=${COMP_WORDS[i+1]}
                break
            fi
        done

        # before we scan for targets, see if a sconsfile name was
        # specified with -f/--file/--sconsfile
        for (( i=0; i < ${#COMP_WORDS[@]}; i++ )); do
            if [[ ${COMP_WORDS[i]} == -@(f|-?(make)file) ]]; then
                # eval for tilde expansion
                eval sconsf=${COMP_WORDS[i+1]}
                break
            fi
        done

        [ -n "$sconsf" ] && sconsf="-f ${sconsf}"
        [ -n "$sconsf_dir" ] && sconsf_dir="-C ${sconsf_dir}"

        COMPREPLY=( $( compgen -W "$( scons -h $sconsf $sconsf_dir 2>/dev/null | \
python -c '

from __future__ import print_function
import sys
import re

targetsHeaderLineStartWords = [ "Targets", "Clean targets", "Utils targets" ]
ignoredLineStartWords = [ "scons:", "default:", "actual:", "Use scons" ]

printTargets = False

def lineStartWithAny(line, startWordsList):
	for startWords in startWordsList:
		if re.match("^"+startWords, line):
			return True
	return False

while True:
	line = sys.stdin.readline()
	if not line:
		break
	
	if line == "":
		continue
		
	if lineStartWithAny(line, ignoredLineStartWords):
		continue
		
	splittedLine = line.split()
	if not printTargets and len(splittedLine) >= 1:
		if splittedLine[0][-1] == ":" and \
				not lineStartWithAny(splittedLine[0], ignoredLineStartWords):
			print(splittedLine[0][:-1])
			continue	
		
	printThisLine = True
			
	if lineStartWithAny(line, targetsHeaderLineStartWords):
		printTargets = True
		printThisLine = False

				
	if printThisLine and printTargets:
		if len(splittedLine) >= 1:
			print(splittedLine[0])

')" -- "$cur" ) )

    fi
}
complete -F _scons scons

# Local variables:
# mode: shell-script
# sh-basic-offset: 4
# sh-indent-comment: t
# indent-tabs-mode: nil
# End:
# ex: ts=4 sw=4 et filetype=sh
