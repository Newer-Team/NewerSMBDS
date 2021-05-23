nsub_020E933C_ov_0A:
    stmfd sp!, {r4,lr}
    mov r0, #0x3F4
    bl allocFromGameHeap
    mov r4, r0
    cmp r4, #0
    beq ActorSpawner_ctor_end
    bl EnemyActor_ctor
    ldr r0, =ActorSpawner_vtable
    str r0, [r4]
ActorSpawner_ctor_end:
    mov r0, r4
    ldmfd sp!, {r4,pc}
    
ActorSpawner_init:
    stmfd sp!, {r4,lr}
    mov r4, r0

    @ is that really needed? dunno.
    ldr r2, =mainHeapHandle
    ldr r2, [r2]
    mov r1, #0x40
    ldr r3, [r0]
    ldr r3, [r3,#0x34]
    blx r3
    
    mov r0, r4
    bl ActorSpawner_setup
    
    mov r0, #1
    ldmfd sp!, {r4,pc}

ActorSpawner_vtable:
    .long ActorSpawner_init
    .long actor_beforeCreate
    .long actor_afterCreate
    .long base_onDelete
    .long actor_beforeDelete
    .long actor_afterDelete
    .long enemyActor_execute
    .long enemyActor_beforeExecute
    .long enemyActor_afterExecute
    .long 0x0209CAC0
    .long enemyActor_beforeDraw
    .long actor_afterDraw
    .long base_willBeDeleted
    .long base_moreHeapShit
    .long base_createHeap
    .long base_heapCreated
    .long 0x020D5894
    .long 0x020D58F0
    .long actor_getSomething
    .long actor_incrementSomething
    .long ActorSpawner_update_state0
    .long 0x0209AD1C
    .long enemyActor_executeState1
    .long enemyActor_executeState2
    .long enemyActor_executeState3
    .long enemyActor_executeState4
    .long enemyActor_executeState5
    .long enemyActor_executeState6
    .long enemyActor_executeState7
    .long enemyActor_executeState8
    .long enemyActor_executeState9
    .long 0x0209CAC0
    .long 0x0209FAF4
    .long 0x0209FAAC
    .long 0x0209C974
    .long 0x020A012C
    .long 0x020A00AC
    .long 0x0209F574    @ sets state=9
    .long 0x0209F3D8
    .long 0x0209F354    @ sets state=0
    .long enemyActor_executeAllStates
    .long 0x0209CAC0            @ called when hit from below block (EnemyActor__virt41: 0x0209D988) 
    .long 0x0209CAC0            @ called when recieving a koopa shell (EnemyActor__virt42: 0x0209D920)
    .long 0x0209CAC0            @ called when recieving a fireball (EnemyActor__virt43: 0x0209D84C)
    .long 0x0209CAC0            @ unknown callback
    .long 0x0209CAC0            @ unknown callback
    .long 0x0209CAC0            @ called when killed by Mega Mario (EnemyActor__virt47: 0x0209D158)
    .long 0x0209CAC0            @ unknown callback
    .long 0x0209CAC0            @ unknown callback
    .long 0x0209CAC0            @ called when beeing jumped on
    .long 0x0209CAC0            @ called when beeing ground-pounded
    .long 0x0209CAC0            @ called when hit by Shell Mario
    .long 0x0209CAD0            @ unknown callback
    .long 0x0209CAC0            @ unknown callback
    .long 0x0209CAC0            @ called when player touches flagpole (EnemyActor__DieFromFlagpole: 0x02098E7C)