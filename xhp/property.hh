<?hh // strict
/**
 * Hphpdoc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @copyright 2016 Appertly
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */

use FredEmmott\DefinitionFinder\ScannedProperty;

/**
 * Renders a property.
 */
class :hphpdoc:property extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;
    use Hphpdoc\Producer;

    category %flow, %sectioning;
    children empty;
    attribute :section,
        ScannedProperty property @required;

    protected function render(): XHPRoot
    {
        $m = $this->:property;
        $sc = $this->getContext('scannedClass');
        invariant($sc instanceof \FredEmmott\DefinitionFinder\ScannedBase, "scannedClass context value must be a ScannedBase");
        $parser = $this->getContext('docParser');
        if (!($parser instanceof Hphpdoc\Doc\Parser)) {
            $parser = new Hphpdoc\Doc\Parser();
        }
        $mdParser = $this->getContext('markdownParser');
        if (!($mdParser instanceof League\CommonMark\DocParser)) {
            $mdParser = new League\CommonMark\DocParser(
                League\CommonMark\Environment::createCommonMarkEnvironment()
            );
        }
        $phpdoc = $parser->parse($m);
        $summary = trim($phpdoc->getSummary());
        $rt = Vector{$m->getTypehint()};
        foreach ($phpdoc->getTags() as $t) {
            if ($t->getName() === 'var' && $t instanceof Hphpdoc\Doc\TypedTag) {
                if ($rt[0] === null || $rt[0]?->getTypeName() === 'mixed') {
                    $rt = $t->getTypes();
                }
                if (strlen($summary) === 0) {
                    $summary = $t->getDescription();
                }
                break;
            }
        }
        return <section id={"property_" . $m->getName()} class="property-section">
            <header>
                <h1>${$m->getName()}</h1>
            </header>
            <div class="property-signature">
                {$m->isStatic() ? 'static ' : ''}
                {$m->isPublic() ? 'public ' : ''}
                {$m->isProtected() ? 'protected ' : ''}
                <code class="property-type">
                    <hphpdoc:typehints tokens={$rt}/>
                </code>
                {" "}
                <code class="property-name"><var>${$m->getName()}</var></code>
            </div>
            <div class="property-details">
                <div class="property-summary"><axe:markdown text={$summary} docParser={$mdParser}/></div>
                <div class="property-description">
                    <axe:markdown text={$phpdoc->getDescription()} docParser={$mdParser}/>
                </div>
                <hphpdoc:authorship block={$phpdoc}/>
                <hphpdoc:versions block={$phpdoc}/>
                <hphpdoc:links block={$phpdoc}/>
            </div>
        </section>;
    }
}