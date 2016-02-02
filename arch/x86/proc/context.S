              .global task_load_context
task_load_context:
              push  %ebp
              mov   %esp,%ebp
              sub   $0x8,%esp
              mov   0x8(%ebp),%eax
              mov   %eax,-0x4(%ebp)
              movl  $0x0,-0x8(%ebp)
              mov   -0x4(%ebp),%eax

              mov   0x18(%eax),%edx
              mov   0x1c(%eax),%ebx
              mov   0x20(%eax),%ecx
              mov   %edx,%esp
              mov   %ebx,%ebp
              mov   %ecx,%cr3

              movl  $0x1,-0x8(%ebp)

              mov   -0x8(%ebp),%eax
              add   $0x8,%esp
              pop   %ebp
              ret


              .global task_store_context
task_store_context:
              push  %ebp
              mov   %esp,%ebp
              sub   $0x8,%esp
              mov   0x8(%ebp),%eax
              mov   %eax,-0x4(%ebp)
              movl  $0x0,-0x8(%ebp)
              mov   -0x4(%ebp),%eax

              mov   %esp,%edx
              mov   %ebp,%ebx
              mov   %edx,0x18(%eax)
              mov   %ebx,0x1c(%eax)

              mov   -0x8(%ebp),%eax
              add   $0x8,%esp
              pop   %ebp
              ret

              .extern pg_clone_dir
              .global task_fork_context
task_fork_context:
              push  %ebp
              mov   %esp,%ebp
              sub   $0x8,%esp
              mov   0x8(%ebp),%eax
              mov   %eax,-0x4(%ebp)
              movl  $0x0,-0x8(%ebp)
              mov   -0x4(%ebp),%eax

              mov   %esp,%edx
              mov   %ebp,%ebx
              mov   %edx,0x18(%eax)
              mov   %ebx,0x1c(%eax)
              mov   %cr3,%ebx
              push  %eax
              push  %ebx
              call  pg_clone_dir
              add   $4,%esp
              mov   %eax,%ecx
              pop   %eax
              mov   %ecx,0x20(%eax)

              mov   -0x8(%ebp),%eax
              add   $0x8,%esp
              pop   %ebp
              ret
