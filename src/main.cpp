#include "rose.h"

class SynthesizedAttribute
{
public:
    std::vector<SgInitializedName *> globalStatics;
    std::unordered_map<SgFunctionDefinition *, std::vector<SgInitializedName *>> worklist;

    void accumulate(const SynthesizedAttribute &other)
    {
        for (auto e : other.globalStatics)
        {
            globalStatics.push_back(e);
        }
        for (auto kv : other.worklist)
        {
            worklist.insert(kv);
        }
    }
};

class visitorTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
{
public:
    // virtual function must be defined
    virtual SynthesizedAttribute evaluateSynthesizedAttribute(SgNode *n, SynthesizedAttributesList childAttributes);
};

SynthesizedAttribute
visitorTraversal::evaluateSynthesizedAttribute(SgNode *n, SynthesizedAttributesList childAttributes)
{
    SynthesizedAttribute localResult;

    for (SynthesizedAttribute child : childAttributes)
    {
        localResult.accumulate(child);
    }

    if (SgVarRefExp *vre = isSgVarRefExp(n))
    {
        SgVariableSymbol *sym = vre->get_symbol();
        SgInitializedName *name = sym->get_declaration();
        if (isSgGlobal(name->get_scope()) && name->get_storageModifier().isStatic())
        {
            localResult.globalStatics.push_back(name);
        }
    }
    else if (SgFunctionDefinition *fundef = isSgFunctionDefinition(n))
    {
        if (localResult.globalStatics.size() > 0)
        {
            localResult.worklist.insert({fundef, localResult.globalStatics});
            std::cout << "FunDef: " << fundef->get_declaration()->get_name() << std::endl;
            for (SgInitializedName *name : localResult.globalStatics)
            {
                std::cout << "  Referenced global static: " << name->get_name() << std::endl;
            }
        }
    }

    return localResult;
}

class UpdateVarRefs : public AstSimpleProcessing
{
    std::unordered_map<SgInitializedName *, SgInitializedName *> replacements;

public:
    UpdateVarRefs(std::unordered_map<SgInitializedName *, SgInitializedName *> replacements) : replacements(replacements) {}

    virtual void visit(SgNode *n);
};

void UpdateVarRefs::visit(SgNode *n)
{
    namespace SB = SageBuilder;

    if (SgVarRefExp *vre = isSgVarRefExp(n))
    {
        SgInitializedName *name = vre->get_symbol()->get_declaration();
        if (replacements.find(name) != replacements.end())
        {
            std::cout << "Updating var ref: " << vre->get_symbol()->get_declaration()->get_name() << std::endl;
            SgInitializedName *replacement = replacements[name];
            if (SgVariableSymbol *sym = isSgVariableSymbol(replacement->search_for_symbol_from_symbol_table()))
            {
                vre->set_symbol(sym);
            }
        }
    }
}

int main(int argc, char **argv)
{
    ROSE_INITIALIZE;
    SgProject *project = frontend(argc, argv);

    // Build the traversal object and call "traverse" member function
    visitorTraversal exampleTraversal;

    // the synthesized attribute will contain a worklist containing all functions to transform.
    // we will need to transform all of the function definitions as well as all call sites to them.
    SynthesizedAttribute results = exampleTraversal.traverse(project);

    for (auto workitem : results.worklist)
    {
        SgFunctionDefinition *fundef = workitem.first;
        std::cout << "Rewriting parameters for " << fundef->get_declaration()->get_name() << std::endl;
        std::vector<SgInitializedName *> vars = workitem.second;

        SgFunctionParameterList *params = fundef->get_declaration()->get_parameterList();

        std::unordered_map<SgInitializedName *, SgInitializedName *> replacements;

        for (SgInitializedName *gvar : vars)
        {
            std::cout << "Adding param : " << gvar->get_name() << std::endl;
            SgInitializedName *newparam = SageBuilder::buildInitializedName(gvar->get_name(), gvar->get_type());
            SageInterface::appendArg(params, newparam);
            replacements.insert({gvar, newparam});
        }

        UpdateVarRefs(replacements).traverse(fundef->get_body(), postorder);
    }

    project->unparse();
    return 0;
}