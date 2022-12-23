const fs = require('fs')
const path = require('path')
const {Plugin} = require('release-it')

const basefile = path.join(__dirname, '..', 'include', 'enet.h')
const workdir = path.join(__dirname, 'deploy')

const versionGet = () => {
    const data = fs.readFileSync(basefile, 'utf8')

    const major = data.match(/ENET_VERSION_MAJOR\s+([0-9]+)\n/)[1]
    const minor = data.match(/ENET_VERSION_MINOR\s+([0-9]+)\n/)[1]
    const patch = data.match(/ENET_VERSION_PATCH\s+([0-9]+)\n/)[1]
    const pre   = data.match(/ENET_VERSION_PRE\s+\"([\.a-z0-9]+)\"\n/)

    return `${major}.${minor}.${patch}${pre ? '-' + pre[1] : ''}`
}

const versionSet = (version) => {
    let data = fs.readFileSync(basefile, 'utf8')

    let [base, pre] = version.split('-')
    let [major, minor, patch] = base.split('.').map(a => parseInt(a))

    if (!pre) pre = ''

    data = data.replace(/ENET_VERSION_MAJOR\s+([0-9]+)\n/, `ENET_VERSION_MAJOR ${major}\n`)
    data = data.replace(/ENET_VERSION_MINOR\s+([0-9]+)\n/, `ENET_VERSION_MINOR ${minor}\n`)
    data = data.replace(/ENET_VERSION_PATCH\s+([0-9]+)\n/, `ENET_VERSION_PATCH ${patch}\n`)
    data = data.replace(/ENET_VERSION_PRE\s+\"([\.0-9a-z]+)\"\n/, `ENET_VERSION_PRE "${pre}"\n`)

    fs.writeFileSync(basefile, data)
}

const embedIncludes = (print) => {
    if (!fs.existsSync(workdir)) fs.mkdirSync(workdir)

    let data = fs.readFileSync(basefile, 'utf8')
    let lines = data.split('\n')

    // const hedley = lines.find(a => a.indexOf('enet_hedley.h') !== -1)
    // const hedleyIndex = lines.indexOf(hedley)

    // lines = lines.map((line, i) => {
    //     if (i < hedleyIndex) return line
    //     if (line.indexOf('#include') === -1) return line
    //     if (line.indexOf('<') !== -1) return line

    //     const parts = line.split('#include')
    //     const spaces = parts[0]
    //     const filename = parts[1].trim().replace(/"/g, '')

    //     const content = fs
    //         .readFileSync(path.join(__dirname, '..', 'code', filename), 'utf8')
    //         .split('\n')
    //         .map(l => spaces + l)
    //         .map(l => l === spaces ? '' : l)
    //         .join('\n')
    //         .replace(/\s+$/g, '')

    //     return content
    // })

    const code = lines.join('\n')
    if (print) console.log(code)
    else fs.writeFileSync(path.join(workdir, 'enet.h'), code)
}

class Bumper extends Plugin {
    getLatestVersion() {
        return versionGet()
    }

    bump(version) {
        this.version = version;
        versionSet(version)
    }

    async beforeRelease() {
        embedIncludes()
        console.log('done')
    }

    afterRelease() {
        if (fs.existsSync(path.join(workdir, 'enet.h'))) {
            fs.unlinkSync(path.join(workdir, 'enet.h'))
        }
    }
}

module.exports = Bumper
module.exports.embedIncludes = embedIncludes
