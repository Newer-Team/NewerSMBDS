
import struct

from . import _common


class NSBMD:
    """
    A 3D model.
    """
    def __init__(self, data=None):
        if data is not None:
            self._initFromData(data)

    def _initFromData(self, data):
        if not data.startswith(b'BMD0'):
            raise ValueError('Invalid NSBMD: incorrect magic')

        magic, bom, version, filesize, headersize, numblocks = \
            _common.NDS_STD_FILE_HEADER.unpack_from(data, 0)
        if version != 2:
            raise ValueError(f'Unsupported NSBMD version: {version}')

        blockOffsets = struct.unpack_from(f'<{numblocks}I', data, 0x10)
        for off in blockOffsets:
            blockMagic, blockLen = struct.unpack_from('<4sI', data, off)
            blockData = data[off : off+blockLen]
            print('BLOCK OFF ' + blockMagic.decode('ascii') + ' ' + hex(off))

            if blockMagic == b'MDL0':
                self._parseMDL0(blockData)
            elif blockMagic == b'TEX0':
                self._parseTEX0(blockData)
            else:
                raise ValueError(f'Unknown NSBMD block: {blockMagic}')


    def _parseMDL0(self, data):
        
        modelsCount, info3dSize = struct.unpack_from('<xBH', data, 0x8)

        off = 0x0C
        unkBlockHeaderLen, unkBlockLen, unkBlockUnk04 = struct.unpack_from('<HHI', data, off)
        assert unkBlockHeaderLen == 8
        assert unkBlockUnk04 == 0x17F
        unkBlockEntries = []
        for i in range(modelsCount):
            unk00, unk02 = struct.unpack_from('<HH', data, off + 0x8 + 4 * i)
            unkBlockEntries.append((unk00, unk02))

        off += 8 + 4 * modelsCount # unkBlockLen is a lie?
        infoSectionLen, infoLen = struct.unpack_from('<HH', data, off)
        modelOffsets = []
        for i in range(modelsCount):
            modelOff, = struct.unpack_from('<I', data, off + 0x4 + 4 * i)
            modelOffsets.append(modelOff)

        off += 4 + 4 * modelsCount
        modelNames = []
        for i in range(modelsCount):
            name = data[off : off+16].rstrip(b'\0').decode('latin-1')
            modelNames.append(name)
            off += 16

        self.models = []
        for (unk00, unk02), modelOff, modelName in zip(unkBlockEntries, modelOffsets, modelNames):
            (modelSize, bonesOffset, materialsOffset,
                    polygonsBeginOffset, polygonsEndOffset,
                    unk14, unk15, unk16, objectsCount, materialsCount,
                    polygonsCount, unk1A, unk1B, unk1C, unk1D, unk1E, unk1F,
                    unk20, scaleMode, unk22, unk23, verticesCount,
                    surfacesCount, trianglesCount, quadsCount,
                    boundingBoxX, boundingBoxY, boundingBoxZ,
                    boundingBoxWidth, boundingBoxHeight, boundingBoxDepth,
                    runtimeUse) = \
                struct.unpack_from('<5I16B4H6h8s', data, modelOff)
            print('RUNTIME USE @ ' + hex(modelOff + 0x38))
            print(runtimeUse)

            # Bones
            print('BONES @ ' + hex(modelOff + bonesOffset))

            # Materials
            print('MATERIALS @ ' + hex(modelOff + materialsOffset))
            off = modelOff + materialsOffset + 4 # TODO: WHY DO WE HAVE TO ADD 4
            materialsCount, materialsHeaderSize = struct.unpack_from('<xBH', data, off)
            off += 4

            unkMatBlockHeaderLen, unkMatBlockLen, unkMatBlockUnk04 = \
                struct.unpack_from('<HHI', data, off)
            off += 8
            assert unkMatBlockHeaderLen == 8
            assert unkMatBlockUnk04 == 0x17F
            unkMatBlockEntries = []
            for i in range(materialsCount):
                unk00, unk02 = struct.unpack_from('<HH', data, off)
                unkMatBlockEntries.append((unk00, unk02))
                off += 4

            matOffsetsHeaderLen, matOffsetsLen = \
                struct.unpack_from('<HH', data, off)
            off += 4
            assert matOffsetsHeaderLen == 4
            matOffsets = []
            for i in range(materialsCount):
                matOffsets.append(struct.unpack_from('<I', data, off)[0])
                off += 4

            matNames = []
            for i in range(materialsCount):
                name = data[off : off+16].rstrip(b'\0').decode('latin-1')
                matNames.append(name)
                off += 16

            texturesCount, texturesHeaderSize = struct.unpack_from('<xBH', data, off)
            off += 4

            unkTexBlockHeaderLen, unkTexBlockLen, unkTexBlockUnk04 = \
                struct.unpack_from('<HHI', data, off)
            off += 8
            assert unkTexBlockHeaderLen == 8
            assert unkTexBlockUnk04 == 0x17F
            unkTexBlockEntries = []
            for i in range(texturesCount):
                unk00, unk02 = struct.unpack_from('<HH', data, off)
                unkTexBlockEntries.append((unk00, unk02))
                off += 4

            texDatasHeaderLen, texDatasLen = \
                struct.unpack_from('<HH', data, off)
            off += 4
            assert texDatasHeaderLen == 4
            texDatas = []
            for i in range(texturesCount):
                texDatas.append(struct.unpack_from('<HBx', data, off))
                off += 4

            texNames = []
            for i in range(texturesCount):
                name = data[off : off+16].rstrip(b'\0').decode('latin-1')
                texNames.append(name)
                off += 16

            palettesCount, palettesHeaderSize = struct.unpack_from('<xBH', data, off)
            off += 4

            unkPalBlockHeaderLen, unkPalBlockLen, unkPalBlockUnk04 = \
                struct.unpack_from('<HHI', data, off)
            off += 8
            assert unkPalBlockHeaderLen == 8
            assert unkPalBlockUnk04 == 0x17F
            unkPalBlockEntries = []
            for i in range(palettesCount):
                unk00, unk02 = struct.unpack_from('<HH', data, off)
                unkPalBlockEntries.append((unk00, unk02))
                off += 4

            palDatasHeaderLen, palDatasLen = \
                struct.unpack_from('<HH', data, off)
            off += 4
            assert palDatasHeaderLen == 4
            palDatas = []
            for i in range(palettesCount):
                palDatas.append(struct.unpack_from('<HBx', data, off))
                off += 4

            palNames = []
            for i in range(palettesCount):
                name = data[off : off+16].rstrip(b'\0').decode('latin-1')
                palNames.append(name)
                off += 16

            print(palNames, hex(off + 0x18))

            for (unk00, unk02), matOffset, matName in zip(unkMatBlockEntries, matOffsets, matNames):
                off = modelOff + materialsOffset + matOffset + 4
                print(hex(off + 0x18))


            # Polygons
            print('POLYGONS @ ' + hex(modelOff + polygonsBeginOffset))
            off = modelOff + polygonsBeginOffset
            polygonsCount, polygonsHeaderSize = struct.unpack_from('<xBH', data, off)
            off += 4

            unkPolyBlockHeaderLen, unkPolyBlockLen, unkPolyBlockUnk04 = \
                struct.unpack_from('<HHI', data, off)
            off += 8
            assert unkPolyBlockHeaderLen == 8
            assert unkPolyBlockUnk04 == 0x17F
            unkPolyBlockEntries = []
            for i in range(polygonsCount):
                unk00, unk02 = struct.unpack_from('<HH', data, off)
                unkPolyBlockEntries.append((unk00, unk02))
                off += 4

            polyOffsetsHeaderLen, polyOffsetsLen = \
                struct.unpack_from('<HH', data, off)
            off += 4
            assert polyOffsetsHeaderLen == 4
            polyOffsets = []
            for i in range(polygonsCount):
                polyOffsets.append(struct.unpack_from('<I', data, off)[0])
                off += 4

            polyNames = []
            for i in range(polygonsCount):
                name = data[off : off+16].rstrip(b'\0').decode('latin-1')
                polyNames.append(name)
                off += 16

            for (unk00, unk02), polyOffset, polyName in zip(unkPolyBlockEntries, polyOffsets, polyNames):
                polyDefOffset = modelOff + polygonsBeginOffset + polyOffset
                unkDef00, unkDef04, displayListOff, displayListLen = \
                    struct.unpack_from('<4I', data, polyDefOffset)

                displayListData = data[polyDefOffset + displayListOff : polyDefOffset + displayListOff + displayListLen]
                displayList = self._parseDisplayList(displayListData)
                # for x in displayList: print(x)
                # print('--------')

            model = None
            self.models.append((modelName, model))

        #print(self.models)

        self._MDL0 = data


    @staticmethod
    def _parseDisplayList(data):
        commands = []
        i = 0
        while i < len(data):
            commandIDs = data[i : i+4]
            i += 4
            for id in commandIDs:
                cmdType = _COMMANDS_BY_ID.get(id)
                if cmdType is None:
                    print(f'WARNING: Unknown command ID {hex(id)}!')
                    cmdType = GeometryCommand
                    paramCount = 0
                else:
                    paramCount = cmdType.PARAM_COUNT
                params = []
                for j in range(paramCount):
                    params.append(data[i : i+4])
                    i += 4
                cmd = cmdType.fromData(params)
                cmd.type = cmdType # in case this command has an unknown ID
                commands.append(cmd)
        return commands


    def _parseTEX0(self, data):
        self._TEX0 = data


    def _saveMDL0(self):
        return self._MDL0


    def _saveTEX0(self):
        return self._TEX0


    def save(self):

        data = bytearray(0x10)

        # Gather data blocks
        dataBlocks = []
        dataBlocks.append(self._saveMDL0())
        dataBlocks.append(self._saveTEX0())

        # Add space for the block offsets
        data.extend(b'\0' * (len(dataBlocks) * 4))

        # Add the block datas and insert the offsets into the table
        for i, block in enumerate(dataBlocks):
            struct.pack_into('<I', data, 0x10 + 4 * i, len(data))
            data.extend(block)

        # Add the NDS standard file header
        _common.NDS_STD_FILE_HEADER.pack_into(data, 0,
            b'BMD0', 0xFEFF, 2, len(data), 0x10, 2)

        # And return.
        return bytes(data)






class GeometryCommand():
    type = None
    PARAM_COUNT = 0

    @classmethod
    def fromData(cls, params):
        return cls()


class GeometryCommand_Nop(GeometryCommand):
    type = 0


class GeometryCommand_SetMatrixMode(GeometryCommand):
    """
    Also known as MTX_MODE.
    """
    type = 0x10
    PARAM_COUNT = 1


class GeometryCommand_PushMatrix(GeometryCommand):
    """
    Also known as MTX_PUSH.
    """
    type = 0x11
    PARAM_COUNT = 0


class GeometryCommand_PopMatrix(GeometryCommand):
    """
    Also known as MTX_POP.
    """
    type = 0x12
    PARAM_COUNT = 1


class GeometryCommand_StoreMatrix(GeometryCommand):
    """
    Also known as MTX_STORE.
    """
    type = 0x13
    PARAM_COUNT = 1


class GeometryCommand_RestoreMatrix(GeometryCommand):
    """
    Also known as MTX_RESTORE.
    """
    type = 0x14
    PARAM_COUNT = 1


class GeometryCommand_LoadIdentityMatrix(GeometryCommand):
    """
    Also known as MTX_IDENTITY.
    """
    type = 0x15
    PARAM_COUNT = 0


class GeometryCommand_Load4x4Matrix(GeometryCommand):
    """
    Also known as MTX_LOAD_4x4.
    """
    type = 0x16
    PARAM_COUNT = 16


class GeometryCommand_Load4x3Matrix(GeometryCommand):
    """
    Also known as MTX_LOAD_4x3.
    """
    type = 0x17
    PARAM_COUNT = 12


class GeometryCommand_MultiplyBy4x4Matrix(GeometryCommand):
    """
    Also known as MTX_MULT_4x4.
    """
    type = 0x18
    PARAM_COUNT = 16


class GeometryCommand_MultiplyBy4x3Matrix(GeometryCommand):
    """
    Also known as MTX_MULT_4x3.
    """
    type = 0x19
    PARAM_COUNT = 12


class GeometryCommand_MultiplyBy3x3Matrix(GeometryCommand):
    """
    Also known as MTX_MULT_3x3.
    """
    type = 0x1A
    PARAM_COUNT = 9


class GeometryCommand_MultiplyByScaleMatrix(GeometryCommand):
    """
    Also known as MTX_SCALE.
    """
    type = 0x1B
    PARAM_COUNT = 3


class GeometryCommand_MultiplyByTranslationMatrix(GeometryCommand):
    """
    Also known as MTX_TRANS.
    """
    type = 0x1C
    PARAM_COUNT = 3


class GeometryCommand_SetVertexColor(GeometryCommand):
    """
    Also known as COLOR.
    """
    type = 0x20
    PARAM_COUNT = 1


class GeometryCommand_SetNormalVector(GeometryCommand):
    """
    Also known as NORMAL.
    """
    type = 0x21
    PARAM_COUNT = 1


class GeometryCommand_SetTextureCoordinates(GeometryCommand):
    """
    Also known as TEXCOORD.
    """
    type = 0x22
    PARAM_COUNT = 1


class GeometryCommand_SetVertexXYZCoordinates_16BitsEach(GeometryCommand):
    """
    Also known as VTX_16.
    """
    type = 0x23
    PARAM_COUNT = 2


class GeometryCommand_SetVertexXYZCoordinates_10BitsEach(GeometryCommand):
    """
    Also known as VTX_10.
    """
    type = 0x24
    PARAM_COUNT = 1


class GeometryCommand_SetVertexXYCoordinates(GeometryCommand):
    """
    Also known as VTX_XY.
    """
    type = 0x25
    PARAM_COUNT = 1


class GeometryCommand_SetVertexXZCoordinates(GeometryCommand):
    """
    Also known as VTX_XZ.
    """
    type = 0x26
    PARAM_COUNT = 1


class GeometryCommand_SetVertexYZCoordinates(GeometryCommand):
    """
    Also known as VTX_YZ.
    """
    type = 0x27
    PARAM_COUNT = 1


class GeometryCommand_SetRelativeVertexCoordinates(GeometryCommand):
    """
    Also known as VTX_DIFF.
    """
    type = 0x28
    PARAM_COUNT = 1


class GeometryCommand_SetPolygonAttributes(GeometryCommand):
    """
    Also known as POLYGON_ATTR.
    """
    type = 0x29
    PARAM_COUNT = 1


class GeometryCommand_SetTextureParameters(GeometryCommand):
    """
    Also known as TEXIMAGE_PARAM.
    """
    type = 0x2A
    PARAM_COUNT = 1


class GeometryCommand_SetTexturePaletteBaseAddress(GeometryCommand):
    """
    Also known as PLTT_BASE.
    """
    type = 0x2B
    PARAM_COUNT = 1


class GeometryCommand_SetDiffuseAndAmbientReflection(GeometryCommand):
    """
    Also known as DIF_AMB.
    """
    type = 0x30
    PARAM_COUNT = 1


class GeometryCommand_SetSpecularReflectionAndEmission(GeometryCommand):
    """
    Also known as SPE_EMI.
    """
    type = 0x31
    PARAM_COUNT = 1


class GeometryCommand_SetLightDirectionalVector(GeometryCommand):
    """
    Also known as LIGHT_VECTOR.
    """
    type = 0x32
    PARAM_COUNT = 1


class GeometryCommand_SetLightColor(GeometryCommand):
    """
    Also known as LIGHT_COLOR.
    """
    type = 0x33
    PARAM_COUNT = 1


class GeometryCommand_SetSpecularReflectionShininessTable(GeometryCommand):
    """
    Also known as SHININESS.
    """
    type = 0x34
    PARAM_COUNT = 32


class GeometryCommand_StartOfVertexList(GeometryCommand):
    """
    Also known as BEGIN_VTXS.
    """
    type = 0x40
    PARAM_COUNT = 1


class GeometryCommand_EndOfVertexList(GeometryCommand):
    """
    Also known as END_VTXS.
    This seems to have no effect on hardware.
    """
    type = 0x41
    PARAM_COUNT = 0


class GeometryCommand_SwapRenderingEngineBuffers(GeometryCommand):
    """
    Also known as SWAP_BUFFERS.
    """
    type = 0x50
    PARAM_COUNT = 1


class GeometryCommand_SetViewport(GeometryCommand):
    """
    Also known as VIEWPORT.
    """
    type = 0x60
    PARAM_COUNT = 1


class GeometryCommand_TestIfCuboidSitsInsideViewVolume(GeometryCommand):
    """
    Also known as BOX_TEST.
    """
    type = 0x70
    PARAM_COUNT = 3


class GeometryCommand_SetPositionCoordinatesForTest(GeometryCommand):
    """
    Also known as POS_TEST.
    """
    type = 0x71
    PARAM_COUNT = 2


class GeometryCommand_SetDirectionalVectorForTest(GeometryCommand):
    """
    Also known as VEC_TEST.
    """
    type = 0x72
    PARAM_COUNT = 3


_COMMANDS_BY_ID = {
    0x00: GeometryCommand_Nop,
    0x10: GeometryCommand_SetMatrixMode,
    0x11: GeometryCommand_PushMatrix,
    0x12: GeometryCommand_PopMatrix,
    0x13: GeometryCommand_StoreMatrix,
    0x14: GeometryCommand_RestoreMatrix,
    0x15: GeometryCommand_LoadIdentityMatrix,
    0x16: GeometryCommand_Load4x4Matrix,
    0x17: GeometryCommand_Load4x3Matrix,
    0x18: GeometryCommand_MultiplyBy4x4Matrix,
    0x19: GeometryCommand_MultiplyBy4x3Matrix,
    0x1A: GeometryCommand_MultiplyBy3x3Matrix,
    0x1B: GeometryCommand_MultiplyByScaleMatrix,
    0x1C: GeometryCommand_MultiplyByTranslationMatrix,
    0x20: GeometryCommand_SetVertexColor,
    0x21: GeometryCommand_SetNormalVector,
    0x22: GeometryCommand_SetTextureCoordinates,
    0x23: GeometryCommand_SetVertexXYZCoordinates_16BitsEach,
    0x24: GeometryCommand_SetVertexXYZCoordinates_10BitsEach,
    0x25: GeometryCommand_SetVertexXYCoordinates,
    0x26: GeometryCommand_SetVertexXZCoordinates,
    0x27: GeometryCommand_SetVertexYZCoordinates,
    0x28: GeometryCommand_SetRelativeVertexCoordinates,
    0x29: GeometryCommand_SetPolygonAttributes,
    0x2A: GeometryCommand_SetTextureParameters,
    0x2B: GeometryCommand_SetTexturePaletteBaseAddress,
    0x30: GeometryCommand_SetDiffuseAndAmbientReflection,
    0x31: GeometryCommand_SetSpecularReflectionAndEmission,
    0x32: GeometryCommand_SetLightDirectionalVector,
    0x33: GeometryCommand_SetLightColor,
    0x34: GeometryCommand_SetSpecularReflectionShininessTable,
    0x40: GeometryCommand_StartOfVertexList,
    0x41: GeometryCommand_EndOfVertexList,
    0x50: GeometryCommand_SwapRenderingEngineBuffers,
    0x60: GeometryCommand_SetViewport,
    0x70: GeometryCommand_TestIfCuboidSitsInsideViewVolume,
    0x71: GeometryCommand_SetPositionCoordinatesForTest,
    0x72: GeometryCommand_SetDirectionalVectorForTest,
}
