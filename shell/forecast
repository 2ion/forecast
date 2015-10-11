#!/bin/bash
_forecast ()
{
  local cur=${COMP_WORDS[COMP_CWORD]}
  local prev=${COMP_WORDS[COMP_CWORD-1]}

  if [[ -n "$prev" ]]; then
    case "$prev" in
      -c|--config) _filedir; return;;
      --language) COMPREPLY=($(compgen -W "ar bs de el en es fr hr it nl pl pt ru sk tet tr uk x-pig-latin zh zh-tw" -- "$cur")); return;;
      -m|--mode) COMPREPLY=($(compgen -W "print print-hourly plot-hourly plot-daily plot-precip-daily plot-precip-hourly" -- "$cur")); return;;
      -u|--units) COMPREPLY=($(compgen -W "si us uk ca auto" -- "$cur")); return;;
      -s|--step|-L|--location-by-name|-l|--location) return;;
    esac
  fi

  if [[ -z "$cur" || $cur =~ ^- ]]; then
    COMPREPLY=($(compgen -W "-c --config -d --dump -e --extended-hourly
    -h --help -L --location-by-name -l --location --language -m --mode
    -r --request -s --step -u --units -v --version" -- "$cur"))
    return
  fi
}
complete -F _forecast forecast
