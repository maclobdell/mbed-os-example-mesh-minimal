properties ([[$class: 'ParametersDefinitionProperty', parameterDefinitions: [
  [$class: 'StringParameterDefinition', name: 'mbed_os_revision', defaultValue: 'master', description: 'Revision of mbed-os to build'],
  [$class: 'BooleanParameterDefinition', name: 'smoke_test', defaultValue: true, description: 'Enable to run HW smoke test after building']
  ]]])

echo "Run smoke tests: ${smoke_test}"

try {
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
} catch (err) {
  def mbed_os_revision = "master"
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
}

// Map RaaS instances to corresponding test suites
def raas = [
  "lowpan_mesh_minimal_smoke_k64f_atmel.json": "8001",
  "lowpan_mesh_minimal_smoke_k64f_mcr20.json": "8034",
  "lowpan_mesh_minimal_smoke_429zi_atmel.json": "8030",
  "lowpan_mesh_minimal_smoke_429zi_mcr20.json": "8033",
  "lowpan_mesh_minimal_smoke_ublox_atmel.json": "8031",
  "thread_mesh_minimal_smoke_k64f_atmel.json": "8007",
  "thread_mesh_minimal_smoke_k64f_mcr20.json": "8034",
  "thread_mesh_minimal_smoke_429zi_atmel.json": "8030",
  "thread_mesh_minimal_smoke_429zi_mcr20.json": "8033",
  "thread_mesh_minimal_smoke_ublox_atmel.json": "8031"
  ]

// List of targets with supported RF shields to compile
def targets = [
  "K64F": ["ATMEL", "MCR20"],
  //"NUCLEO_F401RE": ["ATMEL", "MCR20"],
  "NUCLEO_F429ZI": ["ATMEL", "MCR20"],
  //"NCS36510": ["NCS36510"],
  "UBLOX_EVK_ODIN_W2": ["ATMEL"]
  ]
  
// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc",
  IAR: "iar_arm"
  ]

// Supported RF shields
def radioshields = [
  "ATMEL",
  "MCR20",
  "NCS36510"
  ]

// Mesh interfaces: 6LoWPAN and Thread
def meshinterfaces = [
  "6lp",
  "thd"
  ]
  
def stepsForParallel = [:]

// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < targets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    for(int k = 0; k < radioshields.size(); k++) {
      for(int l = 0; l < meshinterfaces.size(); l++) {
        def target = targets.keySet().asList().get(i)
        def allowed_shields = targets.get(target)
        def toolchain = toolchains.keySet().asList().get(j)
        def compilerLabel = toolchains.get(toolchain)
        def radioshield = radioshields.get(k)
        def meshInterface = meshinterfaces.get(l)
        def stepName = "${target} ${toolchain} ${radioshield} ${meshInterface}"
        if(allowed_shields.contains(radioshield)) {
          stepsForParallel[stepName] = buildStep(target, compilerLabel, toolchain, radioshield, meshInterface)
        }
      }
    }
  }
}

//def parallelRunSmoke = [:]

// Need to compare boolean against string value
if ( smoke_test == "true" ) {
  // Generate smoke tests based on suite amount
  for(int i = 0; i < raas.size(); i++) {
    def suite_to_run = raas.keySet().asList().get(i)
    def raasPort = raas.get(suite_to_run)
    def smokeStep = "${raasPort} ${suite_to_run.substring(0, suite_to_run.indexOf('.'))}"
    echo "Smoke step: ${smokeStep}"
    def parallelRunSmoke[smokeStep] = run_smoke(targets, toolchains, radioshields, meshinterfaces, raasPort, suite_to_run)
  }
}

timestamps {
  parallel stepsForParallel
  parallelRunSmoke
}

def buildStep(target, compilerLabel, toolchain, radioShield, meshInterface) {
  return {
    stage ("${target}_${compilerLabel}_${radioShield}_${meshInterface}") {
      node ("${compilerLabel}") {
        deleteDir()
        step([$class: 'WsCleanup'])
      }
    }
  }
}

def run_smoke(targets, toolchains, radioshields, meshinterfaces, raasPort, suite_to_run) {
  return {
    def suiteName = suite_to_run.substring(0, suite_to_run.indexOf('.'))
    echo "Stage: smoke_${raasPort}_${suiteName}"
    stage ("smoke_${raasPort}_${suiteName}") {
      node ("linux_test") {
        deleteDir()
        for (int i = 0; i < targets.size(); i++) {
          for(int j = 0; j < toolchains.size(); j++) {
            for(int k = 0; k < radioshields.size(); k++) {
              for(int l = 0; l < meshinterfaces.size(); l++) {
                def target = targets.keySet().asList().get(i)
                def allowed_shields = targets.get(target)
                def toolchain = toolchains.keySet().asList().get(j)
                def radioshield = radioshields.get(k)
                def meshInterface = meshinterfaces.get(l)
                if(allowed_shields.contains(radioshield)) {
                  echo "Unstashing: ${target}_${toolchain}_${radioshield}_${meshInterface}"
                  //unstash "${target}_${toolchain}_${radioshield}_${meshInterface}"
                }
              }
            }
          }
        }
        echo "log_${raasPort}_${suiteName}"
      }
    }
  }
}
