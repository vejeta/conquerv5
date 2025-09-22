# Start Alpine container with the APK available
docker run --rm -it -v "$PWD":/work alpine:latest sh -c "
  # Install dependencies and the game
  apk add --no-cache ncurses-terminfo-base ncurses
  apk add --allow-untrusted /work/packages/x86_64/conquerv5-4.12-r0.apk
  
  # Set up environment
  export TERM=xterm
  export HOME=/tmp/conquer-test
  mkdir -p \$HOME
  
  # Show what's available
  echo '=== Game installed! Available commands: ==='
  echo 'conquer -h    # Game help'
  echo 'conqrun -h    # Admin help'
  echo 'conquer       # Start the game'
  echo 'conqrun -m    # Create a world (admin)'
  echo ''
  
  # Drop into interactive shell
  /bin/sh
"
