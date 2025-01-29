
bool compareTwoRealVariant(const ofec::Real& data1, const ofec::Real& data2) {
    std::string info1, info2;
    std::stringstream tmp;
    tmp << data1;
    info1 = tmp.str();
    tmp.str("");

    //ofec::Real tmp;

    tmp << data2;
    info2 = tmp.str();
    tmp.clear();

    return info1 == info2;
}

bool compareParameterString(const ofec::ParameterVariant& data1, const ofec::ParameterVariant& data2) {
    if (data1.index() != data2.index())return false;
    auto type = static_cast<ofec::ParameterType>(data1.index());
    if (type == ofec::ParameterType::kReal) {
        return compareTwoRealVariant(std::get<ofec::Real>(data1), std::get<ofec::Real>(data2));
    }
    else if (type == ofec::ParameterType::kVectorReal) {
        auto& vdata1 = std::get<std::vector<ofec::Real>>(data1);
        auto& vdata2 = std::get<std::vector<ofec::Real>>(data2);
        if (vdata1.size() != vdata2.size())return false;
        for (int idx(0); idx < vdata1.size(); ++idx) {
            if (!compareTwoRealVariant(vdata1[idx], vdata2[idx])) {
                return false;
            }
            return true;
        }
    }
    else {
        return data1 == data2;
    }
}

void compare(ofec::ParameterVariantStream& paramsStream1, ofec::ParameterVariantStream& paramsStream2) {
    std::vector<ofec::ParameterVariantStream::Type> vectorParam1;
    std::vector<ofec::ParameterVariantStream::Type> vectorParam2;
    while (!paramsStream1.empty()) {
        vectorParam1.emplace_back(paramsStream1.popVariant());
    }
    while (!paramsStream2.empty()) {
        vectorParam2.emplace_back(paramsStream2.popVariant());
    }

    auto size = std::min(vectorParam1.size(), vectorParam2.size());
    for (int idx(0); idx < size; ++idx) {
        if (idx == 10) {
            int stop = -1;
        }
        if (!compareParameterString(vectorParam1[idx].first, vectorParam2[idx].first)) {
            std::cout << "from\t" << ofec::variants_stream::m_errorInfos[idx].from;
            std::cout << "\tto\t" << ofec::variants_stream::m_errorInfos[idx].to;
            std::cout << "\tThreadId\t" << ofec::variants_stream::m_errorInfos[idx].InfoIdx << std::endl;

            std::cout << "error idx\t" << idx << "\tparameters1 \t";
            ofec::variants_stream::output(std::cout, vectorParam1[idx].first);
            std::cout << std::endl;
            // << vectorParam1[idx].first << std::endl;

            std::cout << "error idx\t" << idx << "\tparameters2 \t";
            //<< vectorParam2[idx].first << std::endl;

            ofec::variants_stream::output(std::cout, vectorParam2[idx].first);
            std::cout << std::endl;

            break;
        }
    }
}


// 采样字符集
static constexpr char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// sz: 字符串的长度
// printable：是否可打印。如果用作key，可以使用不可打印的字符哟
string makeRandStr(int sz, ofec::Random* rnd, bool printable)
{
    string ret;
    ret.resize(sz);
    // std::mt19937 rng(std::random_device{}());
    for (int i = 0; i < sz; ++i)
    {
        auto rndNum = rnd->uniform.nextNonStd<size_t>(std::numeric_limits<size_t>::lowest(), std::numeric_limits<size_t>::max());
        if (printable)
        {

            uint32_t x = rndNum % (sizeof(CCH) - 1);
            ret[i] = CCH[x];
        }
        else
        {
            ret[i] = rndNum % 0xFF;
        }
    }

    return ret;
}

template<typename T>
void setValue(ofec::ParameterVariant& data, ofec::Random* rnd) {
    T    value = rnd->uniform.nextNonStd<T>(std::numeric_limits<T>::lowest() / 3, std::numeric_limits<T>::max() / 3);
    data = value;
}


template <>
void setValue<bool>(ofec::ParameterVariant& data, ofec::Random* rnd) {
    bool value = rnd->uniform.nextNonStd<int>(0, 2);
    data = value;
}

template <>
void setValue<char>(ofec::ParameterVariant& data, ofec::Random* rnd) {
    char value = CCH[rnd->uniform.nextNonStd<int>(0, sizeof(CCH) - 1)];
    data = value;
}

void setString(ofec::ParameterVariant& data, ofec::Random* rnd, size_t maxSize = 100) {
    std::string value = makeRandStr(rnd->uniform.nextNonStd<size_t>(0, maxSize), rnd, true);
    data = value;
}

template<typename T>
void setVectorValue(ofec::ParameterVariant& data, ofec::Random* rnd, size_t maxSize = 100) {
    std::vector<T> values(rnd->uniform.nextNonStd<size_t>(0, maxSize));
    for (auto& value : values) {
        value = rnd->uniform.nextNonStd<T>(std::numeric_limits<T>::lowest() / 3, std::numeric_limits<T>::max() / 3);
    }
    data = values;
}

template<>
void setVectorValue<bool>(ofec::ParameterVariant& data, ofec::Random* rnd, size_t maxSize) {
    std::vector<bool> values(rnd->uniform.nextNonStd<size_t>(0, maxSize));
    for (auto& value : values) {
        value = rnd->uniform.nextNonStd<int>(0, 2);
    }
    data = values;
}



void generateVariants(ofec::ParameterVariant& data, ofec::Random* rnd, size_t maxSize = 100) {
    using namespace ofec;
    int numType = 10;
    auto type = static_cast<ofec::ParameterType>(rnd->uniform.nextNonStd<int>(0, numType));
    switch (type)
    {

    case ParameterType::kBool: {
        setValue<bool>(data, rnd);
        break;
    }
    case ParameterType::kInt: {
        setValue<int>(data, rnd);
        break;
    }
    case ParameterType::kSizeT: {
        setValue<size_t>(data, rnd);
        //info << std::get<size_t>(data) << "\t";
        break;
    }
    case ParameterType::kChar: {
        setValue<char>(data, rnd);
        //info << std::get<char>(data) << "\t";
        break;
    }
    case ParameterType::kReal: {
        setValue<ofec::Real>(data, rnd);
        break;
    }

    case ParameterType::kVectorBool: {
        setVectorValue<bool>(data, rnd, maxSize);

        break;
    }

    case ParameterType::kVectorInt: {
        setVectorValue<int>(data, rnd, maxSize);
        break;
    }

    case ParameterType::kVectorSizeT: {
        setVectorValue<size_t>(data, rnd, maxSize);
        break;
    }

    case ParameterType::kString: {
        setString(data, rnd, maxSize);
        break;
    }

    case ParameterType::kVectorReal: {
        setVectorValue<Real>(data, rnd, maxSize);
        break;
    }
    default:
        break;
    }
}

void generateParameterString(ofec::Random* rnd, ofec::ParameterVariantStream& paramsStream) {
    //    ofec::ParameterVariantStream paramsStream;
    ofec::ParameterVariant data;
    int numData = 100;
    while (numData--) {
        generateVariants(data, rnd);
        paramsStream << data;
    }
}



void testBufReader(std::stringstream& buf, int from, int to) {
    using namespace ofec;
    ofec::variants_stream::ThreadLocalInfo info;
    ofec::variants_stream::GlobalThreadInfo globalInfo;
    globalInfo.m_info = buf.str();
    info.from = from;
    info.to = to;
    variants_stream::str2DataOneTaskLine(globalInfo, info);
}

void testParametersStringInputOutput() {
    auto rnd = std::make_shared<ofec::Random>(0.5);
    ofec::ParameterVariantStream paramsStreamOut;
    ofec::ParameterVariantStream paramsStreamIn;
    generateParameterString(rnd.get(), paramsStreamOut);
    std::stringstream buf;
    ofec::ParameterVariantStream tmp = paramsStreamOut;
    ofec::variants_stream::parameters2StreamMutithread(buf, tmp);

    std::cout << "output buf size\t" << buf.str().size() << std::endl;
    {

        std::ofstream out("testParamters.txt");
        out << buf.rdbuf();
        out.close();
    }
    //std::cout << "tmp buf size\t" << buf.str().size() << std::endl;
    //buf.clear();
    //std::cout << "tmp buf size\t" << buf.str().size() << std::endl;


    //std::stringstream buf2;
    //{
    //    std::ifstream in("testParamters.txt");
    //    buf2 << in.rdbuf();
    //    in.close();
    //}
    //std::cout << "tmp buf size\t" << buf2.str().size() << std::endl;

    //std::cout << "input buf size\t" << buf2.str().size() << std::endl;

    //auto info = buf.str().substr(1, 1470);
    //
  // testBufReader(buf, 3304,4956);


  // ofec::variants_stream::stream2ParametersMutithreadLine(buf, paramsStreamIn);
  //  compare(paramsStreamOut, paramsStreamIn);

}




