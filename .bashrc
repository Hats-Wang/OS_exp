# Source default setting
[ -f /etc/bashrc ] && . /etc/bashrc

# User environment PATH
PATH="$HOME/.local/bin:$HOME/bin:$PATH"
export PATH
