{
  "targets": [
    {
      "target_name": "roboticMouse",
      "sources": [ "roboticMouse.cc" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ]
    }
  ]
}